#pragma once

#include "imdb_search_engine.hpp"

#include <array>
#include <corecrt.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <vector>

#include "boolean_model.hpp"
#include "index_builder.hpp"
#include "spelling_corrector.hpp"

namespace {

std::vector<std::string> GetShowsName() {
  std::ifstream ifs("Index/show_names.txt");
  std::vector<std::string> show_name;
  while (!ifs.eof()) {
    std::string line;
    getline(ifs, line);
    show_name.push_back(line);
  }
  ifs.close();
  std::cout << "Got shows' name!" << std::endl;
  return show_name;
}

std::vector<int> InvertInvertedIndex(Index &index) {
  std::vector<int> res;
  for (auto &x : index) {
    for (auto &y : (*x.second)) {
      if (y >= res.size()) {
        res.resize(y + 1);
      }
      res[y] = stoi(x.first);
    }
  }
  return res;
}

std::vector<QueryType> QueryTypes() {
  return {QueryType::kName,  QueryType::kActor, QueryType::kCharacter,
          QueryType::kQuote, QueryType::kYear,  QueryType::kRating};
}

} // namespace

void IMDbSearchEngine::RegisterFunctions() {
  string_query_ = {{QueryType::kName, &IMDbSearchEngine::NameQuery},
                   {QueryType::kActor, &IMDbSearchEngine::ActorsQuery},
                   {QueryType::kCharacter, &IMDbSearchEngine::CharactersQuery},
                   {QueryType::kQuote, &IMDbSearchEngine::QuotesQuery}};

  numeric_query_ = {{QueryType::kName, &IMDbSearchEngine::YearRangeQuery},
                    {QueryType::kName, &IMDbSearchEngine::RatingRangeQuery}};
}

IMDbSearchEngine::IMDbSearchEngine() {
  std::cout << "Search Engine initialization started!" << std::endl;
  time_t construction_start = clock();
  RegisterFunctions();
  for (const QueryType &query_type : QueryTypes()) {
    indexes_[query_type] = IndexBuilder::BuildIndex(query_type);
    spelling_correctors_.emplace(query_type, indexes_[query_type]);
  }
  show_name_ = GetShowsName();
  show_year_ = InvertInvertedIndex(indexes_[QueryType::kYear]);
  show_rating_ = InvertInvertedIndex(indexes_[QueryType::kRating]);
  time_t construction_end = clock();

  double construction_time =
      (construction_end - construction_start + 0.0) / CLOCKS_PER_SEC;

  std::cout << "Search Engine was initialized in " << construction_time << "s."
            << std::endl;
}

// Static
IMDbSearchEngine &IMDbSearchEngine::GetInstance() {
  static IMDbSearchEngine imdb_search_engine;
  return imdb_search_engine;
}

std::vector<std::string> IMDbSearchEngine::ExecuteQuery(
    const std::map<QueryType, std::set<std::string>> &string_queries_,
    const std::map<QueryType, std::array<int, 2>> &numeric_queries_) {
  std::vector<int> query_response_ids;

  bool just_started = true;
  for (auto &[query_type, queries] : string_queries_) {
    if (!queries.empty()) {
      query_response_ids = just_started
                               ? string_query_[query_type](this, queries)
                               : boolean_model::ListIntersection(
                                     query_response_ids,
                                     string_query_[query_type](this, queries));
      just_started = false;
    }
  }
  for (auto &[query_type, queries] : numeric_queries_) {
    int min_val =
        query_type == QueryType::kYear ? default_min_year : default_min_rating;
    int max_val =
        query_type == QueryType::kYear ? default_max_year : default_max_rating;
    if (queries[0] != min_val && queries[1] != max_val) {
      query_response_ids =
          just_started
              ? numeric_query_[query_type](this, queries[0], queries[1])
              : boolean_model::ListIntersection(
                    query_response_ids,
                    numeric_query_[query_type](this, queries[0], queries[1]));
      just_started = false;
    }
  }

  if (just_started) {
    return {"Empty Query: Please provide info to search for."};
  }

  if (query_response_ids.empty()) {
    return {"No Results were found."};
  }

  // Sorting the results based on rating.
  // TODO(jihad): Improve ranking logic.
  sort(query_response_ids.rbegin(), query_response_ids.rend(),
       [&](int idA, int idB) { return show_rating_[idA] < show_rating_[idB]; });

  std::vector<std::string> query_response;
  for (size_t i = 0;
       i < std::min(max_results_per_query, query_response_ids.size()); i++) {
    query_response.push_back(
        show_name_[query_response_ids[i]] + " (" +
        std::to_string(show_year_[query_response_ids[i]]) + ") (" +
        std::to_string(show_rating_[query_response_ids[i]]) + ")");
  }
  return query_response;
}

std::vector<int>
IMDbSearchEngine::NameQuery(const std::set<std::string> &name) {
  return PhrasesQuery(name, QueryType::kName);
}

std::vector<int>
IMDbSearchEngine::ActorsQuery(const std::set<std::string> &actors) {
  return PhrasesQuery(actors, QueryType::kActor);
}

std::vector<int>
IMDbSearchEngine::CharactersQuery(const std::set<std::string> &characters) {
  return PhrasesQuery(characters, QueryType::kCharacter);
}

std::vector<int>
IMDbSearchEngine::QuotesQuery(const std::set<std::string> &quotes) {
  return PhrasesQuery(quotes, QueryType::kQuote);
}

std::vector<int> IMDbSearchEngine::YearRangeQuery(int L, int R) {
  return boolean_model::GetInRange(L, R, show_year_);
}

std::vector<int> IMDbSearchEngine::RatingRangeQuery(int L, int R) {
  return boolean_model::GetInRange(L, R, show_rating_);
}

std::vector<int> IMDbSearchEngine::PhraseQuery(const std::string &query,
                                               const QueryType &query_type) {
  Index &index = indexes_[query_type];
  std::vector<std::string> words;
  std::stringstream ss(query);
  for (std::string word = ""; ss >> word;) {
    words.push_back(word);
  }
  if (words.size() == 1) { // Fallback to single index logic.
    return index.count(words[0])
               ? *index[words[0]]
               : spelling_correctors_[query_type].CorrectSpelling(query, index);
  }
  std::vector<int> matches = index.count(words[0] + " " + words[1])
                                 ? *(index[words[0] + " " + words[1]])
                                 : std::vector<int>();
  for (size_t i = 2; i < words.size() && !matches.empty(); ++i) {
    matches = boolean_model::ListIntersection(
        matches, *index[words[i - 1] + " " + words[i]]);
  }
  return matches.empty()
             ? spelling_correctors_[query_type].CorrectSpelling(query, index)
             : matches;
}

std::vector<int>
IMDbSearchEngine::PhrasesQuery(const std::set<std::string> &queries,
                               const QueryType &query_type) {
  std::vector<int> res = PhraseQuery(*queries.begin(), query_type);
  for (auto &query : queries) {
    if (query != *queries.begin()) {
      res =
          boolean_model::ListIntersection(res, PhraseQuery(query, query_type));
    }
  }
  return res;
}
