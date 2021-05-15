#pragma once 
#include "Indexing.h"
#include "BooleanModel.h"
#include "SpellingCorrection.h"

int main() {
	initializeIndices();
	initializeTries();
	string q = "cram hemsworth";
	vector<int> res = spellingCorrection(q, ACTOR_INDEX);
	for (auto& x : res) cout << x << ' '; cout << '\n';
	cin.ignore(2); return 0;
}