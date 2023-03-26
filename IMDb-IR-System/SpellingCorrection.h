#pragma once

#include "Indexing.h"
#include "boolean_model.h"
#include <queue>
#include <set>

std::map<char, int> characterMap;
std::vector<Index *> index = {&nameIndex, &actorIndex, &characterIndex,
                              &quoteIndex};

#define NAME_INDEX 0
#define ACTOR_INDEX 1
#define CHARACTER_INDEX 2
#define QUOTE_INDEX 3

struct TrieNode {
  bool b = false;
  std::vector<TrieNode *> nextNode;
  TrieNode(int i) : nextNode(37, nullptr) {}
};

void insertWord(TrieNode *root, const std::string &s, const int &i) {
  for (const char &c : s)
    if (!characterMap.count(c))
      return;

  TrieNode *cur = root;
  for (const char &c : s) {
    int idx = characterMap[c];
    if (cur->nextNode[idx] == nullptr)
      cur->nextNode[idx] = new TrieNode(i);
    cur = cur->nextNode[idx];
  }
  cur->b = true;
}

std::vector<TrieNode *> tries;

void initializeTries() {
  int id = 0;
  characterMap['\''] = 36;
  for (char c = 'a'; c <= 'z'; c++)
    characterMap[c] = id++;
  for (char c = '0'; c <= '9'; c++)
    characterMap[c] = id++;

  for (int i = 0; i < 4; i++) {
    tries.push_back(new TrieNode(i));
    for (auto &x : *index[i])
      insertWord(tries[i], x.first, i);
  }
}


struct PQObj {
  std::vector<int> r;
  std::string word = "";
  PQObj(std::vector<int> _r, std::string _w) : r(_r), word(_w) {}
  bool operator<(const PQObj &rhs) const { return r.size() > rhs.r.size(); }
};

void findAlternatives(const std::string &original, std::string &cur,
                      TrieNode *curNode, const int &ID,
                      std::vector<std::set<std::string>> &alternatives,
                      int i = 0, int ed = 2) {
  if (i == (int)original.size()) {
    if (ed == 0) {
      if (curNode->b)
        alternatives[ID].insert(cur);
      return;
    }
    findAlternatives(original, cur, curNode, ID, alternatives, i, ed - 1);
    for (auto &c : characterMap) {
      if (curNode->nextNode[c.second] != nullptr) {
        cur.push_back(c.first);
        findAlternatives(original, cur, curNode->nextNode[c.second], ID,
                         alternatives, i, ed - 1);
        cur.pop_back();
      }
    }
    return;
  }

  if (ed == 0) { // Advance to the next character
    if (curNode->nextNode[characterMap[original[i]]] == nullptr) {
      return;
    }
    cur.push_back(original[i]);
    findAlternatives(original, cur,
                     curNode->nextNode[characterMap[original[i]]], ID,
                     alternatives, i + 1, 0);
    cur.pop_back();
    return;
  }

  findAlternatives(original, cur, curNode, ID, alternatives, i,
                   ed - 1); // Discard an operation
  findAlternatives(original, cur, curNode, ID, alternatives, i + 1,
                   ed - 1); // Delete current character
  for (auto &c : characterMap)
    if (curNode->nextNode[c.second] != nullptr) {
      cur.push_back(c.first);
      bool dif = c.first != original[i];
      findAlternatives(original, cur, curNode->nextNode[c.second], ID,
                       alternatives, i, ed - 1); // Insert Character
      findAlternatives(original, cur, curNode->nextNode[c.second], ID,
                       alternatives, i + 1,
                       ed - dif); // Replace Character
      cur.pop_back();
    }
}

void generatePhrases(std::priority_queue<PQObj> &top5, std::vector<int> curList,
                     std::string curP, std::string prev, Index &curIndex,
                     std::vector<std::set<std::string>> &alternatives,
                     int i = 1) {
  if (curList.empty()) {
    return;
  }
  if (i == alternatives.size()) {
    if ((int)top5.size() < 5) {
      top5.push(PQObj(curList, curP));
      return;
    }
    PQObj obj(curList, curP);
    if (obj < top5.top()) {
      top5.pop();
      top5.push(obj);
    }
    return;
  }

  for (auto &x : alternatives[i])
    if (curIndex.count(prev + " " + x))
      generatePhrases(
          top5,
          boolean_model::listIntersection(curList, *curIndex[prev + " " + x]),
          curP + x + " ", x, curIndex, alternatives, i + 1);
}

std::vector<int> spellingCorrection(const std::string &query, int t) {
  std::stringstream ss(query);
  std::string word, curFA;
  int id = 0;
  std::vector<std::set<std::string>> alternatives;
  while (ss >> word) {
                curFA = "";
                alternatives.emplace_back();
                findAlternatives(word, curFA, tries[t], id++, alternatives);
  }

  std::priority_queue<PQObj> top5;
  for (auto &x : alternatives[0])
                generatePhrases(top5, (*(*index[t])[x]), x + " ", x, *index[t],
                                alternatives);

  std::vector<PQObj> suggestions;
  while (!top5.empty()) {
                suggestions.push_back(top5.top());
                top5.pop();
  }
  reverse(suggestions.begin(), suggestions.end());

  std::cout << "Query \"" + query + "\": Did you mean:\n";
  int i = 0;
  for (auto &x : suggestions)
                std::cout << "[" << ++i << "]: " << x.word << '\n';

        std::cout << "[" << ++i << "]: None\n";
        int j;
        std::cin >> j;
        if (j >= i || j < 1) {
                return std::vector<int>();
        }
        return move(suggestions[j - 1].r);
}
