#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

using Index = std::map<std::string, std::unique_ptr<std::vector<int>>>;

namespace boolean_model {

// Finds the intersection of two lists `A` and `B`. Assumes the lists contain
// distinct elements.
std::vector<int> ListIntersection(const std::vector<int> &A,
                                  const std::vector<int> &B);

//
std::vector<int> GetInRange(int L, int R, const std::vector<int> &index);

} // namespace boolean_model
