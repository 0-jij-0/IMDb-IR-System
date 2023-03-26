#include "search_engine_controller.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "imdb_search_engine.hpp"

namespace {

std::string LowercaseString(std::string s) {
  for (char &c : s) {
    if (c >= 'A' && c <= 'Z') {
      c = c - 'A' + 'a';
    }
  }
  return s;
}

} // namespace

void SearchEngineController::RegisterFunctions() {
  edit_query_operation_ = {
      {"@SetName", &SearchEngineController::SetName},
      {"@AddActor", &SearchEngineController::AddActor},
      {"@RemoveActor", &SearchEngineController::RemoveActor},
      {"@AddCharacter", &SearchEngineController::AddCharacter},
      {"@RemoveCharacter", &SearchEngineController::RemoveCharacter},
      {"@AddQuote", &SearchEngineController::AddQuote},
      {"@RemoveQuote", &SearchEngineController::RemoveQuote},
      {"@SetMinYear", &SearchEngineController::SetMinYear},
      {"@SetMaxYear", &SearchEngineController::SetMaxYear},
      {"@SetMinRating", &SearchEngineController::SetMinRating},
      {"@SetMaxRating", &SearchEngineController::SetMaxRating}};

  clear_query_operation_ = {
      {"@ClearName", &SearchEngineController::ClearName},
      {"@ClearActor", &SearchEngineController::ClearActors},
      {"@ClearCharacter", &SearchEngineController::ClearCharacters},
      {"@ClearQuote", &SearchEngineController::ClearQuotes},
      {"@ClearYears", &SearchEngineController::ClearYears},
      {"@ClearRatings", &SearchEngineController::ClearRatings},
      {"@ClearAll", &SearchEngineController::ClearAll}};
}

SearchEngineController::SearchEngineController() {
  IMDbSearchEngine::GetInstance();
  RegisterFunctions();
  numeric_queries_[QueryType::kYear] = {IMDbSearchEngine::default_min_year,
                                        IMDbSearchEngine::default_max_year};
  numeric_queries_[QueryType::kRating] = {IMDbSearchEngine::default_min_rating,
                                          IMDbSearchEngine::default_max_rating};
}

SearchEngineController &SearchEngineController::GetInstance() {
  static SearchEngineController search_engine_controller;
  return search_engine_controller;
}

bool SearchEngineController::ExecuteQuery(const std::string &query) {
  auto query_name_end = find(query.begin(), query.end(), ' ');
  std::string query_name = std::string(query.begin(), query_name_end);
  if (query_name == "@Exit") {
    return false;
  }
  if (query_name == "@Search") {
    return Search();
  }
  if (!edit_query_operation_.count(query_name) &&
      !clear_query_operation_.count(query_name)) {
    std::cout << "Query " << query_name << " not recognized" << std::endl;
    return true;
  }

  query_name.substr(0, 6) == "@Clear"
      ? clear_query_operation_[query_name](this)
      : edit_query_operation_[query_name](
            this, LowercaseString(std::string(query_name_end, query.end())));
  return true;
}

void SearchEngineController::SetName(const std::string &name) {
  string_queries_[QueryType::kName] = {name};
}

void SearchEngineController::ClearName() {
  string_queries_[QueryType::kName].clear();
}

void SearchEngineController::AddActor(const std::string &actor) {
  string_queries_[QueryType::kActor].insert(actor);
}

void SearchEngineController::RemoveActor(const std::string &actor) {
  if (!string_queries_[QueryType::kActor].count(actor)) {
    std::cout << "The actor '" << actor << "' is not in your query."
              << std::endl;
  } else {
    string_queries_[QueryType::kActor].erase(actor);
  }
}

void SearchEngineController::ClearActors() {
  string_queries_[QueryType::kActor].clear();
}

void SearchEngineController::AddCharacter(const std::string &character) {
  string_queries_[QueryType::kCharacter].insert(character);
}

void SearchEngineController::RemoveCharacter(const std::string &character) {
  if (!string_queries_[QueryType::kCharacter].count(character)) {
    std::cout << "The character '" << character << "' is not in your query."
              << std::endl;
  } else {
    string_queries_[QueryType::kCharacter].erase(character);
  }
}

void SearchEngineController::ClearCharacters() {
  string_queries_[QueryType::kCharacter].clear();
}

void SearchEngineController::AddQuote(const std::string &quote) {
  string_queries_[QueryType::kQuote].insert(quote);
}

void SearchEngineController::RemoveQuote(const std::string &quote) {
  if (!string_queries_[QueryType::kQuote].count(quote)) {
    std::cout << "The quote '" << quote << "' is not in your query."
              << std::endl;
  } else {
    string_queries_[QueryType::kQuote].erase(quote);
  }
}

void SearchEngineController::ClearQuotes() {
  string_queries_[QueryType::kQuote].clear();
}

void SearchEngineController::SetMinYear(const std::string &min_year) {
  numeric_queries_[QueryType::kYear][0] = stoi(min_year);
}

void SearchEngineController::SetMaxYear(const std::string &max_year) {
  numeric_queries_[QueryType::kYear][1] = stoi(max_year);
}

void SearchEngineController::ClearYears() {
  numeric_queries_[QueryType::kYear] = {IMDbSearchEngine::default_min_year,
                                        IMDbSearchEngine::default_max_year};
}

void SearchEngineController::SetMinRating(const std::string &min_rating) {
  numeric_queries_[QueryType::kRating][0] = stoi(min_rating);
}

void SearchEngineController::SetMaxRating(const std::string &max_rating) {
  numeric_queries_[QueryType::kRating][0] = stoi(max_rating);
}

void SearchEngineController::ClearRatings() {
  numeric_queries_[QueryType::kYear] = {IMDbSearchEngine::default_min_year,
                                        IMDbSearchEngine::default_max_year};
}

void SearchEngineController::ClearAll() {
  ClearName();
  ClearActors();
  ClearCharacters();
  ClearQuotes();
  ClearYears();
  ClearRatings();
}

bool SearchEngineController::Search() {
  std::vector<std::string> query_response =
      IMDbSearchEngine::GetInstance().ExecuteQuery(string_queries_,
                                                   numeric_queries_);
  for (const std::string &response : query_response) {
    std::cout << response << '\n';
  }
  std::cout << std::endl;
  ClearAll();
  return true;
}
