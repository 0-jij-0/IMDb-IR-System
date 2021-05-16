#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <time.h>
#include <iomanip>
using namespace std;
typedef long long ll;
typedef double ld;

vector<string> showName;
vector<int> showRating, showYear;
map<string, vector<int>> nameIndex;
map<string, vector<int>> actorIndex;
map<string, vector<int>> characterIndex;
map<string, vector<int>> quoteIndex;
map<string, vector<int>> yearIndex;
map<string, vector<int>> ratingIndex;

void getShowsName() {
	ifstream ifs("ShowNameMap.txt"); while (!ifs.eof()) {
		string line; getline(ifs, line);
		showName.emplace_back(line);
	} ifs.close();
}

void getBooleanInvertedIndex(string fileName, map<string, vector<int>> &index) {
	ifstream ifs(fileName.c_str()); while (!ifs.eof()) {
		string line; getline(ifs, line); if (line.empty()) { continue; }
		stringstream ss(line); string key; ss >> key;
		int ID; while (ss >> ID) { index[key].push_back(ID); }
	} ifs.close();
}

void getShowsInfo(map<string, vector<int>> &index, vector<int>& info) {
	info.resize(showName.size()); for (auto& x : index) 
		for(auto &y : x.second) info[y] = stoi(x.first);
}

void initializeIndices() {
	getShowsName();
	getBooleanInvertedIndex("NameIndex.txt", nameIndex);
	getBooleanInvertedIndex("ActorIndex.txt", actorIndex);
	getBooleanInvertedIndex("CharacterIndex.txt", characterIndex);
	getBooleanInvertedIndex("YearIndex.txt", yearIndex);
	getBooleanInvertedIndex("RatingIndex.txt", ratingIndex);
	getBooleanInvertedIndex("QuoteIndex.txt", quoteIndex);
	getShowsInfo(yearIndex, showYear); 
	getShowsInfo(ratingIndex, showRating);
}