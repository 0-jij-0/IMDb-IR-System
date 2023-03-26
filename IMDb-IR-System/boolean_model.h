#pragma once

#include <string>
#include <vector>

#include "indexing.h"

namespace boolean_model {

// Finds the intersection of two lists `A` and `B`. Assumes the lists contain
// distinct elements.
std::vector<int> listIntersection(const std::vector<int> &A,
                                  const std::vector<int> &B);

// Given a sentence `query` and an Index `index`, finds the index of appearance
// of all the words in `query` simultaneously
std::vector<int> ANDQuery(std::vector<std::string> &query, Index &index);

//
std::vector<int> listsUnion(int L, int R, std::vector<int> &index);

} // namespace boolean_model
