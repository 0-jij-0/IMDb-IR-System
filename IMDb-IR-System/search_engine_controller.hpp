#include <array>
#include <functional>
#include <set>
#pragma once

#include <map>
#include <string>

#include "query_types.hpp"

class SearchEngineController {
public:
  static SearchEngineController &GetInstance();

  bool ExecuteQuery(const std::string &query);

private:
  SearchEngineController();
  SearchEngineController(const SearchEngineController &) = delete;
  SearchEngineController operator=(const SearchEngineController &) = delete;

  void SetName(const std::string &name);
  void ClearName();

  void AddActor(const std::string &actor);
  void RemoveActor(const std::string &actor);
  void ClearActors();

  void AddCharacter(const std::string &character);
  void RemoveCharacter(const std::string &character);
  void ClearCharacters();

  void AddQuote(const std::string &quote);
  void RemoveQuote(const std::string &quote);
  void ClearQuotes();

  void SetMinYear(const std::string &min_year);
  void SetMaxYear(const std::string &max_year);
  void ClearYears();

  void SetMinRating(const std::string &min_rating);
  void SetMaxRating(const std::string &max_rating);
  void ClearRatings();

  void ClearAll();

  bool Search();

  void RegisterFunctions();

  std::map<QueryType, std::set<std::string>> string_queries_;
  std::map<QueryType, std::array<int, 2>> numeric_queries_;

  std::map<std::string,
           std::function<void(SearchEngineController *, std::string)>>
      edit_query_operation_;
  std::map<std::string, std::function<void(SearchEngineController *)>>
      clear_query_operation_;
};
