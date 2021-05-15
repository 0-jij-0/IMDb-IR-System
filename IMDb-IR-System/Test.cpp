#pragma once 
#include "Indexing.h"
#include "BooleanModel.h"
#include "SpellingCorrection.h"

int main() {
	initializeSearchEngine();
	string q = "cram hemsworth";
	vector<int> res = spellingCorrection(q, actorIndex);
	for (auto& x : res) cout << x << ' '; cout << '\n';
	cin.ignore(2); return 0;
}