
#include "boolean_model.hpp"

#include <algorithm>

namespace boolean_model {

namespace {

std::vector<int> ListIntersection2P(const std::vector<int> &A,
                                    const std::vector<int> &B) {
  std::vector<int> res;
  for (size_t i = 0, j = 0; i != A.size() && j != B.size(); ++i, ++j) {
    if (A[i] == B[j]) {
      res.push_back(A[i]);
    } else {
      A[i] < B[j] ? --j : --i;
    }
  }
  return res;
}

std::vector<int> ListIntersectionBS(const std::vector<int> &A,
                                    const std::vector<int> &B) {
  std::vector<int> res;
  for (auto itA = A.begin(), itB = B.begin(); itA != A.end() && itB != B.end();
       ++itA) {
    itB = std::lower_bound(itB, B.end(), *itA);
    if (*itB == *itA) {
      res.push_back(*itA);
    }
  }
  return res;
}

} // namespace

std::vector<int> ListIntersection(const std::vector<int> &A,
                                  const std::vector<int> &B) {
  const std::vector<int> &a = (A.size() > B.size()) ? B : A;
  const std::vector<int> &b = (A.size() > B.size()) ? A : B;
  double ratio = (b.size() + 0.0) / (a.size() + 0.0);
  return ratio > 9.0 ? ListIntersectionBS(a, b) : ListIntersection2P(a, b);
}

std::vector<int> GetInRange(int L, int R, const std::vector<int> &index) {
  std::vector<int> res;
  for (int i = 0; i < (int)index.size(); i++)
    if (index[i] >= L && index[i] <= R)
      res.push_back(i);
  return res;
}

} // namespace boolean_model
