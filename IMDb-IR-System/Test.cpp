#pragma once 
#include "Indexing.h"
#include "BooleanModel.h"
#include "SpellingCorrection.h"
#include "Queries.h"

string name = "", quote = ""; 
vector<string> actors, characters; 
int minY = 0, maxY = 3000, minR = 0, maxR = 100;

void initializeSearchEngine() {
	time_t start = clock();
	initializeIndices();
	initializeTries();
	time_t finish = clock();

	double elapsed = 1000.0 * (finish - start) / CLOCKS_PER_SEC;
	cout << fixed << setprecision(3);
	cout << "Initializing the search engine took " << elapsed / 1000.0 << "s" << endl << endl;
}

void executeQuery() {
	bool emp = true; vector<int> res;

	if (name != "" && emp) { res = move(nameQuery(name)); emp = false; }
	else if (name != "") { res = listIntersection(res, nameQuery(name)); }

	if (!actors.empty()) {
		vector<int> ans = actorQuery(actors[0]);
		for (auto& x : actors) if (x != actors[0])
			ans = listIntersection(ans, actorQuery(x));
		if (emp) { res = move(ans); emp = false; }
		else { res = listIntersection(res, ans); }
	}

	if (!characters.empty()) {
		vector<int> ans = characterQuery(characters[0]);
		for (auto& x : characters) if (x != characters[0])
			ans = listIntersection(ans, characterQuery(x));
		if (emp) { res = move(ans); emp = false; }
		else { res = listIntersection(res, ans); }
	}

	if (minY != 0 || maxY != 3000) {
		vector<int> ans = yearQuery(minY, maxY);
		if (emp) { res = move(ans); emp = false; }
		else { res = listIntersection(res, ans); }
	}

	if (minR != 0 || maxR != 100) {
		vector<int> ans = ratingQuery(minR, maxR);
		if (emp) { res = move(ans); emp = false; }
		else { res = listIntersection(res, ans); }
	}

	if (emp) { cout << "Empty Query: Please provide info to search for." << endl << endl; return; }
	if (res.empty()) { cout << "No Results were found." << endl << endl; return; }


}

int main() {
	initializeSearchEngine();

	bool stop = false; while (!stop) {
		string Q; getline(cin, Q);
		int idx = find(Q.begin(), Q.end(), ' ') - Q.begin();

		string type = Q.substr(0, idx);
		string query = (idx != (int)Q.size()) ? Q.substr(idx + 1) : "";

		if (type == "@Name") { name = query; }
		else if (type == "@ClearName") { name = ""; }
		else if (type == "@AddActor") { actors.push_back(query); }
		else if (type == "@ClearActors") { actors.clear(); }
		else if (type == "@AddCharacter") { characters.push_back(query); }
		else if (type == "@ClearCharacters") { characters.clear(); }
		else if (type == "@Quote") { quote = query; }
		else if (type == "@ClearQuote") { quote = ""; }
		else if (type == "@MinYear") { minY = stoi(query); }
		else if (type == "@MaxYear") { maxY = stoi(query); }
		else if (type == "@ClearYears") { minY = 0; maxY = 3000; }
		else if (type == "@MinRating") { minR = (int)floor(10 * stod(query) + 0.1); }
		else if (type == "@MaxRating") { maxR = (int)floor(10 * stod(query) + 0.1); }
		else if (type == "@ClearRatings") { minR = 0, maxR = 100; }
		else if (type == "@ClearAll") { name = quote = ""; minY = minR = 0; maxR = 100; maxY = 3000; actors.clear(); characters.clear(); }
		else if (type == "@Search") { executeQuery(); name = quote = ""; minY = minR = 0; maxR = 100; maxY = 3000; actors.clear(); characters.clear(); }
		else if (type == "@Exit") { stop = true; }
		else { cout << type << ": Query not recognized." << endl; }
	}
}