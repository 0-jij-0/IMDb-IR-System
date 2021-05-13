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

vector<string> movieName;
map<string, vector<int>> nameIndex;
map<string, vector<int>> actorIndex;
map<string, vector<int>> characterIndex;
map<string, map<int, ld>> quoteIndex;
map<string, vector<int>> yearIndex;
map<string, vector<int>> ratingIndex;

void getMoviesName() {
	ifstream ifs("MovieIDMap.txt"); while (!ifs.eof()) {
		string line; getline(ifs, line);
		movieName.emplace_back(line);
	} ifs.close();
}

void getBooleanInvertedIndex(string fileName, map<string, vector<int>> &index) {
	ifstream ifs(fileName.c_str()); while (!ifs.eof()) {
		string line; getline(ifs, line); if (line.empty()) { continue; }
		stringstream ss(line); string key; ss >> key;
		int ID; while (ss >> ID) { index[key].push_back(ID); }
	} ifs.close();
}

void getTFIDFIndex() {
	ifstream ifs("QuoteIndex.txt"); while (!ifs.eof()) {
		string line; getline(ifs, line); if (line.empty()) { continue; }
		stringstream ss(line); string key; ss >> key;
		int ID; ld tfidf; while (ss >> ID) { ss >> tfidf; quoteIndex[key][ID] = tfidf; }
	} ifs.close();
}

void initializeSearchEngine() {
	time_t start = clock(); getMoviesName();
	getBooleanInvertedIndex("NameIndex", nameIndex);
	getBooleanInvertedIndex("ActorIndex", actorIndex);
	getBooleanInvertedIndex("CharacterIndex", characterIndex);
	getBooleanInvertedIndex("YearIndex", yearIndex);
	getBooleanInvertedIndex("RatingIndex", ratingIndex);
	getTFIDFIndex(); time_t finish = clock();

	ld elapsed = 1000.0 * (finish - start) / CLOCKS_PER_SEC;
	cout << "Initializing the Search Engine took " << elapsed << "s" << '\n';
}