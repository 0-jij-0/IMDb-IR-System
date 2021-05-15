#pragma once

#include "Indexing.h"
#include "BooleanModel.h"

vector<set<string>> alternatives;
map<char, int> characterMap;

struct TrieNode {
	bool b = false; vector<TrieNode*> nextNode;
	TrieNode() : nextNode(characterMap.size(), nullptr) {}
};

struct PQObj {
	vector<int> r; string word = "";
	PQObj(vector<int> _r, string _w) : r(_r), word(_w) {}
	bool operator < (const PQObj& rhs) const { return r.size() > rhs.r.size(); }
};

void insertWord(TrieNode* root, const string& s) {
	TrieNode* cur = root; for (const char& c : s) {
		int idx = characterMap[c];
		if (cur->nextNode[idx] == nullptr)
			cur->nextNode[idx] = new TrieNode();
		cur = cur->nextNode[idx];
	}
	cur->b = true;
}

TrieNode* initializeTrie(const map<string, vector<int>>& index) {
	for (auto& x : index) for (const char& y : x.first) characterMap[y];
	int id = 0; for (auto& x : characterMap) x.second = id++;

	TrieNode* root = new TrieNode();
	for (auto& x : index) insertWord(root, x.first);
	return root;
}

void findAlternatives(const string& original, string& cur, TrieNode* curNode, const int& originalID, int i = 0, int ed = 2) {
	if (i == (int)original.size()) {
		if (ed == 0) { if (curNode->b) alternatives[originalID].insert(cur); return; }
		findAlternatives(original, cur, curNode, originalID, i, ed - 1);
		for (auto& c : characterMap) {
			if (curNode->nextNode[c.second] != nullptr) {
				cur.push_back(c.first);
				findAlternatives(original, cur, curNode->nextNode[c.second], originalID, i, ed - 1);
				cur.pop_back();
			}
		} return;
	}

	if (ed == 0) { //Advance to the next character
		if (curNode->nextNode[characterMap[original[i]]] == nullptr) { return; }
		cur.push_back(original[i]);
		findAlternatives(original, cur, curNode->nextNode[characterMap[original[i]]], originalID, i + 1, 0);
		cur.pop_back(); return;
	}

	findAlternatives(original, cur, curNode, originalID, i, ed - 1); //Discard an operation
	if (curNode->nextNode[characterMap[original[i]]] != nullptr) { //Don't perform operations on this character
		cur.push_back(original[i]);
		findAlternatives(original, cur, curNode->nextNode[characterMap[original[i]]], originalID, i + 1, ed);
		cur.pop_back();
	}
	findAlternatives(original, cur, curNode, originalID, i + 1, ed - 1); //Delete current character
	for(auto &c : characterMap) if (curNode->nextNode[c.second] != nullptr) {
		cur.push_back(c.first);
		findAlternatives(original, cur, curNode->nextNode[c.second], originalID, i, ed - 1); //Insert Character
		findAlternatives(original, cur, curNode->nextNode[c.second], originalID, i + 1, ed - 1); //Replace Character
		cur.pop_back();
	}
}

void generatePhrases(priority_queue<PQObj>& top3, vector<int> curList, string curP, map<string, vector<int>> &index, int i = 1) {
	if (curList.empty()) { return; }
	if (i == alternatives.size()) {
		if ((int)top3.size() < 3) { top3.push(PQObj(curList, curP)); return; }
		PQObj obj(curList, curP);
		if (obj < top3.top()) { top3.pop(); top3.push(obj); }
		return;
	}

	for (auto& x : alternatives[i])
		generatePhrases(top3, listIntersection(curList, index[x]), curP + x + " ", index, i + 1);
}

vector<int> spellingCorrection(const string& query, map<string, vector<int>>& index) {
	TrieNode* root = initializeTrie(index);
	stringstream ss(query);	string word, curFA; int id = 0;
	while (ss >> word) {
		curFA = ""; alternatives.emplace_back();
		findAlternatives(word, curFA, root, id++);
	}

	priority_queue<PQObj> top3; for (auto& x : alternatives[0])
		generatePhrases(top3, index[x], x + " ", index);

	vector<PQObj> suggestions; while (!top3.empty()) {
		suggestions.push_back(top3.top()); top3.pop();
	}

	cout << "Query \"" + query + "\": Did you mean:\n";
	int i = 0; for (auto& x : suggestions)
		cout << "[" << ++i << "]: " << x.word << '\n';

	cout << "[" << ++i << "]: None\n";
	int j; cin >> j; if (j >= i || j < 1) { return vector<int>(); }
	return move(suggestions[j - 1].r);
}