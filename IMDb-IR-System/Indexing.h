#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using Index = std::map<std::string, std::unique_ptr<std::vector<int>>>;

enum class IndexType {
  kName = 0,
  kActor = 1,
  kCharacter = 2,
  kQuote = 3,
  kYear = 4,
  kRating = 5
};

std::vector<std::string> showName;
std::vector<int> showRating, showYear;
Index nameIndex;
Index actorIndex;
Index characterIndex;
Index quoteIndex;
Index yearIndex;
Index ratingIndex;

std::map<IndexType, Index> indexes;

void getShowsName() {
  std::ifstream ifs("ShowNames.txt");
  while (!ifs.eof()) {
    std::string line;
    std::getline(ifs, line);
    showName.emplace_back(line);
  }
  ifs.close();
  std::cout << "Got shows names!" << std::endl;
}

void getSingleInvertedIndex(std::string fileName, Index index) {
  std::ifstream ifs(fileName.c_str());
  while (!ifs.eof()) {
    std::string line;
    getline(ifs, line);
    if (line.empty()) {
      continue;
    }
    std::stringstream ss(line);
    std::string key;
    ss >> key;
    if (!index.count(key)) {
      index[key] = std::make_unique<std::vector<int>>();
    }
    for (int ID = 0; ss >> ID;) {
      index[key]->push_back(ID);
    }
  }
  ifs.close();
  std::cout << "Read file " << fileName << std::endl;
}

void getByInvertedIndex(std::string fileName, Index index) {
  std::ifstream ifs(fileName.c_str());
  while (!ifs.eof()) {
    std::string line;
    std::getline(ifs, line);
    if (line.empty()) {
      continue;
    }
    std::stringstream ss(line);
    std::string key1, key2;
    ss >> key1 >> key2;
    std::string key = key1 + " " + key2;
    if (!index.count(key)) {
      index[key] = std::make_unique<std::vector<int>>();
    }
    for (int ID = 0; ss >> ID;) {
      index[key]->push_back(ID);
    }
  }
  ifs.close();
  std::cout << "Read file " << fileName << std::endl;
}

void getShowsInfo(Index &index, std::vector<int> &info,
                  const std::string &info_name) {
  info.resize(showName.size());
  for (auto &x : index)
    for (auto &y : (*x.second))
      info[y] = stoi(x.first);
  std::cout << "Got show's " << info_name << std::endl;
}

void initializeIndices() {
  std::cout << "Start!" << std::endl;
  getShowsName();

  getSingleInvertedIndex("NameIndexS.txt", nameIndex);
  getSingleInvertedIndex("ActorIndexS.txt", actorIndex);
  getSingleInvertedIndex("CharacterIndexS.txt", characterIndex);
  getSingleInvertedIndex("QuoteIndexS.txt", quoteIndex);
  getSingleInvertedIndex("YearIndex.txt", yearIndex);
  getSingleInvertedIndex("RatingIndex.txt", ratingIndex);

  getByInvertedIndex("NameIndexB.txt", nameIndex);
  getByInvertedIndex("ActorIndexB.txt", actorIndex);
  getByInvertedIndex("CharacterIndexB.txt", characterIndex);
  getByInvertedIndex("QuoteIndexB.txt", quoteIndex);

  getShowsInfo(yearIndex, showYear, "year");
  getShowsInfo(ratingIndex, showRating, "rating");
}
