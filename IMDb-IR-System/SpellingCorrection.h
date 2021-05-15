#pragma once

#include "Indexing.h"
#include "BooleanModel.h"

vector<map<char, int>> characterMap(5);
vector<map<string, vector<int>>*> index = { &nameIndex, &actorIndex, &characterIndex, &yearIndex, &ratingIndex };

#define NAME_INDEX 0
#define ACTOR_INDEX 1
#define CHARACTER_INDEX 2
#define YEAR_INDEX 3
#define RATING_INDEX 4


struct TrieNode {
	bool b = false; vector<TrieNode*> nextNode;
	TrieNode(int i) : nextNode(characterMap[i].size(), nullptr) {}
};

void insertWord(TrieNode* root, const string& s, const int& i) {
	TrieNode* cur = root; for (const char& c : s) {
		int idx = characterMap[i][c];
		if (cur->nextNode[idx] == nullptr)
			cur->nextNode[idx] = new TrieNode(i);
		cur = cur->nextNode[idx];
	}
	cur->b = true;
}

vector<TrieNode*> tries;

void initializeTries() {
	for (int i = 0; i < 5; i++) {
		for (auto& x : *index[i]) for (auto& y : x.first) characterMap[i][y];
		int id = 0; for (auto& x : characterMap[i]) x.second = id++;

		tries.push_back(new TrieNode(i)); 
		for (auto& x : *index[i]) insertWord(tries[i], x.first, i);
	}
}


struct PQObj {
	vector<int> r; string word = "";
	PQObj(vector<int> _r, string _w) : r(_r), word(_w) {}
	bool operator < (const PQObj& rhs) const { return r.size() > rhs.r.size(); }
};

void findAlternatives(const string& original, string& cur, TrieNode* curNode, const int& ID, 
	const int& j, vector<set<string>> &alternatives, int i = 0, int ed = 2) {
	if (i == (int)original.size()) {
		if (ed == 0) { if (curNode->b) alternatives[ID].insert(cur); return; }
		findAlternatives(original, cur, curNode, ID, j, alternatives, i, ed - 1);
		for (auto& c : characterMap[j]) {
			if (curNode->nextNode[c.second] != nullptr) {
				cur.push_back(c.first);
				findAlternatives(original, cur, curNode->nextNode[c.second], ID, j, alternatives, i, ed - 1);
				cur.pop_back();
			}
		} return;
	}

	if (ed == 0) { //Advance to the next character
		if (curNode->nextNode[characterMap[j][original[i]]] == nullptr) { return; }
		cur.push_back(original[i]);
		findAlternatives(original, cur, curNode->nextNode[characterMap[j][original[i]]], ID, j, alternatives, i + 1, 0);
		cur.pop_back(); return;
	}

	findAlternatives(original, cur, curNode, ID, j, alternatives, i, ed - 1); //Discard an operation
	findAlternatives(original, cur, curNode, ID, j, alternatives, i + 1, ed - 1); //Delete current character
	for(auto &c : characterMap[j]) if (curNode->nextNode[c.second] != nullptr) {
		cur.push_back(c.first); bool dif = c.first != original[i];
		findAlternatives(original, cur, curNode->nextNode[c.second], ID, j, alternatives, i, ed - 1); //Insert Character
		findAlternatives(original, cur, curNode->nextNode[c.second], ID, j, alternatives, i + 1, ed - dif); //Replace Character
		cur.pop_back();
	}
}

void generatePhrases(priority_queue<PQObj>& top3, vector<int> curList, string curP, 
	map<string, vector<int>> &curIndex, vector<set<string>> &alternatives, int i = 1) {
	if (curList.empty()) { return; }
	if (i == alternatives.size()) {
		if ((int)top3.size() < 3) { top3.push(PQObj(curList, curP)); return; }
		PQObj obj(curList, curP);
		if (obj < top3.top()) { top3.pop(); top3.push(obj); }
		return;
	}

	for (auto& x : alternatives[i])
		generatePhrases(top3, listIntersection(curList, curIndex[x]), curP + x + " ", curIndex, alternatives, i + 1);
}

vector<int> spellingCorrection(const string& query, int t) {
	stringstream ss(query);	string word, curFA; int id = 0;
	vector<set<string>> alternatives; while (ss >> word) {
		curFA = ""; alternatives.emplace_back();
		findAlternatives(word, curFA, tries[t], id++, t, alternatives);
	}

	priority_queue<PQObj> top3; for (auto& x : alternatives[0])
		generatePhrases(top3, (*index[t])[x], x + " ", *index[t], alternatives);

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