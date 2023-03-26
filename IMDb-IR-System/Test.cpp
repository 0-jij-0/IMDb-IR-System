#pragma once

#include <iomanip>

#include "Indexing.h"
#include "Queries.h"
#include "SpellingCorrection.h"
#include "boolean_model.h"


std::string name = "", quote = "";
std::vector<std::string> actors, characters;
int minY = 0, maxY = 3000, minR = 0, maxR = 100;

void initializeSearchEngine() {
	time_t start = clock();
	initializeIndices();
	initializeTries();
	time_t finish = clock();

	double elapsed = 1000.0 * (finish - start) / CLOCKS_PER_SEC;
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Initializing the search engine took " << elapsed / 1000.0
                  << "s" << std::endl
                  << std::endl;
}

std::string lower(std::string s) {
        for (char &c : s)
          if (c >= 'A' && c <= 'Z')
            c = c - 'A' + 'a';
        return s;
}

void executeQuery() {
        bool emp = true;
        std::vector<int> res;

        if (name != "" && emp) {
          res = std::move(nameQuery(name));
          emp = false;
        } else if (name != "") {
          res = boolean_model::listIntersection(res, nameQuery(name));
        }

        if (!actors.empty()) {
          std::vector<int> ans = actorsQuery(actors);
          if (emp) {
            res = std::move(ans);
            emp = false;
          } else {
            res = boolean_model::listIntersection(res, ans);
          }
        }

        if (!characters.empty()) {
          std::vector<int> ans = characterQuery(characters);
          if (emp) {
            res = std::move(ans);
            emp = false;
          } else {
            res = boolean_model::listIntersection(res, ans);
          }
        }

        if (quote != "") {
          std::vector<int> ans = quoteQuery(quote);
          if (emp) {
            res = std::move(ans);
            emp = false;
          } else {
            res = boolean_model::listIntersection(res, ans);
          }
        }

        if (minY != 0 || maxY != 3000) {
          std::vector<int> ans = yearQuery(minY, maxY);
          if (emp) {
            res = std::move(ans);
            emp = false;
          } else {
            res = boolean_model::listIntersection(res, ans);
          }
        }

        if (minR != 0 || maxR != 100) {
          std::vector<int> ans = ratingQuery(minR, maxR);
          if (emp) {
            res = std::move(ans);
            emp = false;
          } else {
            res = boolean_model::listIntersection(res, ans);
          }
        }

        if (emp) {
          std::cout << "Empty Query: Please provide info to search for."
                    << std::endl
                    << std::endl;
          return;
        }
        if (res.empty()) {
          std::cout << "No Results were found." << std::endl << std::endl;
          return;
        }

        auto comp = [&](const int &a, const int &b) {
          return showRating[a] < showRating[b];
        };
        sort(res.rbegin(), res.rend(), comp);

        for (int i = 0; i < std::min(10, (int)res.size()); i++)
          std::cout << showName[res[i]] << " (" << showYear[res[i]] << ") ("
                    << showRating[res[i]] << ")" << std::endl;
        std::cout << std::endl;
}

int main() {
	initializeSearchEngine();

        bool stop = false;
        while (!stop) {
          std::string Q;
          getline(std::cin, Q);
          // cout << Q << '\n';
          int idx = find(Q.begin(), Q.end(), ' ') - Q.begin();

          std::string type = Q.substr(0, idx);
          std::string query =
              (idx != (int)Q.size()) ? lower(Q.substr(idx + 1)) : "";

          if (type == "@Name") {
            name = query;
          } else if (type == "@ClearName") {
            name = "";
          } else if (type == "@AddActor") {
            actors.push_back(query);
          } else if (type == "@ClearActors") {
            actors.clear();
          } else if (type == "@AddCharacter") {
            characters.push_back(query);
          } else if (type == "@ClearCharacters") {
            characters.clear();
          } else if (type == "@Quote") {
            quote = query;
          } else if (type == "@ClearQuote") {
            quote = "";
          } else if (type == "@MinYear") {
            minY = stoi(query);
          } else if (type == "@MaxYear") {
            maxY = stoi(query);
          } else if (type == "@ClearYears") {
            minY = 0;
            maxY = 3000;
          } else if (type == "@MinRating") {
            minR = (int)floor(10 * stod(query) + 0.1);
          } else if (type == "@MaxRating") {
            maxR = (int)floor(10 * stod(query) + 0.1);
          } else if (type == "@ClearRatings") {
            minR = 0, maxR = 100;
          } else if (type == "@ClearAll") {
            name = quote = "";
            minY = minR = 0;
            maxR = 100;
            maxY = 3000;
            actors.clear();
            characters.clear();
          } else if (type == "@Search") {
            executeQuery();
            name = quote = "";
            minY = minR = 0;
            maxR = 100;
            maxY = 3000;
            actors.clear();
            characters.clear();
          } else if (type == "@Exit") {
            stop = true;
          } else if (!type.empty()) {
            std::cout << type << ": Query not recognized." << std::endl;
          }
        }
}
