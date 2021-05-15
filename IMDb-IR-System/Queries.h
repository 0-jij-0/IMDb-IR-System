#pragma once

#include "Indexing.h"
#include "BooleanModel.h"
#include "SpellingCorrection.h"

vector<int> nameQuery(const string& query) {
	vector<string> words; stringstream ss(query);
	string word; while (ss >> word) { words.push_back(word); }

	vector<int> res = ANDQuery(words, nameIndex);
	return res.empty() ? move(spellingCorrection(query, NAME_INDEX)) : move(res);
}

vector<int> actorQuery(const string& query) {
	vector<string> words; stringstream ss(query);
	string word; while (ss >> word) { words.push_back(word); }

	vector<int> res = ANDQuery(words, actorIndex);
	return res.empty() ? move(spellingCorrection(query, ACTOR_INDEX)) : move(res);
}

vector<int> characterQuery(const string& query) {
	vector<string> words; stringstream ss(query);
	string word; while (ss >> word) { words.push_back(word); }

	vector<int> res = ANDQuery(words, characterIndex);
	return res.empty() ? move(spellingCorrection(query, CHARACTER_INDEX)) : move(res);
}

vector<int> yearQuery(int L, int R) {
	return move(listsUnion(L, R, yearIndex));	
}

vector<int> ratingQuery(int L, int R) {
	return move(listsUnion(L, R, ratingIndex));
}