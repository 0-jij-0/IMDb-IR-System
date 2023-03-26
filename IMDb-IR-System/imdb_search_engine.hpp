#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "query_types.hpp"
#include "spelling_corrector.hpp"

using Index = std::map<std::string, std::unique_ptr<std::vector<int>>>;

class IMDbSearchEngine {
public:
  static const size_t max_results_per_query = 10;
  static const int default_min_rating = 0, default_max_rating = 100;
  static const int default_min_year = 0, default_max_year = 1 << 30;

  static IMDbSearchEngine &GetInstance();

  std::vector<std::string> ExecuteQuery(
      const std::map<QueryType, std::set<std::string>> &string_queries_,
      const std::map<QueryType, std::array<int, 2>> &numeric_queries_);

private:
  IMDbSearchEngine();
  IMDbSearchEngine(const IMDbSearchEngine &) = delete;
  IMDbSearchEngine operator=(const IMDbSearchEngine &) = delete;

  std::vector<int> NameQuery(const std::set<std::string> &name);

  std::vector<int> ActorsQuery(const std::set<std::string> &actors);

  std::vector<int> CharactersQuery(const std::set<std::string> &characters);

  std::vector<int> QuotesQuery(const std::set<std::string> &quotes);

  std::vector<int> YearRangeQuery(int L, int R);

  std::vector<int> RatingRangeQuery(int L, int R);

  std::vector<int> PhraseQuery(const std::string &query,
                               const QueryType &query_type);

  std::vector<int> PhrasesQuery(const std::set<std::string> &queries,
                                const QueryType &query_type);

  void RegisterFunctions();

  std::map<QueryType, Index> indexes_;
  std::map<QueryType, SpellingCorrector> spelling_correctors_;
  std::vector<std::string> show_name_;
  std::vector<int> show_rating_;
  std::vector<int> show_year_;

  std::map<QueryType, std::function<std::vector<int>(IMDbSearchEngine *,
                                                     std::set<std::string>)>>
      string_query_;
  std::map<QueryType,
           std::function<std::vector<int>(IMDbSearchEngine *, int, int)>>
      numeric_query_;
};
