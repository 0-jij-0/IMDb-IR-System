#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "query_types.hpp"

using Index = std::map<std::string, std::unique_ptr<std::vector<int>>>;

class IndexBuilder {
public:
  static Index BuildIndex(const QueryType &query_type);

private:
  enum class IndexType { kSingleIndex = 0, kBiIndex = 1 };

  static void ReadInvertedIndex(const std::string &fileName, Index &index,
                                const IndexType &index_type);
};
