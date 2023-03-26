#pragma once

#include "index_builder.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Index IndexBuilder::BuildIndex(const QueryType &query_type) {
  Index index;
  switch (query_type) {
  case QueryType::kName:
    ReadInvertedIndex("Index\\name_single_index.txt", index,
                      IndexType::kSingleIndex);
    ReadInvertedIndex("Index\\name_bi_index.txt", index, IndexType::kBiIndex);
    break;
  case QueryType::kActor:
    ReadInvertedIndex("Index\\actor_single_index.txt", index,
                      IndexType::kSingleIndex);
    ReadInvertedIndex("Index\\actor_bi_index.txt", index, IndexType::kBiIndex);
    break;
  case QueryType::kCharacter:
    ReadInvertedIndex("Index\\character_single_index.txt", index,
                      IndexType::kSingleIndex);
    ReadInvertedIndex("Index\\character_bi_index.txt", index,
                      IndexType::kBiIndex);
    break;
  case QueryType::kQuote:
    ReadInvertedIndex("Index\\quote_single_index.txt", index,
                      IndexType::kSingleIndex);
    ReadInvertedIndex("Index\\quote_bi_index.txt", index, IndexType::kBiIndex);
    break;
  case QueryType::kYear:
    ReadInvertedIndex("Index\\year_index.txt", index, IndexType::kSingleIndex);
    break;
  case QueryType::kRating:
    ReadInvertedIndex("Index\\rating_index.txt", index,
                      IndexType::kSingleIndex);
    break;
  }
  return index;
}

// static
void IndexBuilder::ReadInvertedIndex(const std::string &fileName, Index &index,
                                     const IndexType &index_type) {
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
    if (index_type == IndexType::kBiIndex) {
      std::string temp;
      ss >> temp;
      key += " " + temp;
    }

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
