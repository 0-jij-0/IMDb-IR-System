
#include "boolean_model.h"
#include <algorithm>

namespace boolean_model {

namespace {

std::vector<int> listIntersection2P(const std::vector<int> &A,
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

std::vector<int> listIntersectionBS(const std::vector<int> &A,
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

std::vector<int> listIntersection(const std::vector<int> &A,
                                  const std::vector<int> &B) {
  const std::vector<int> &a = (A.size() > B.size()) ? B : A;
  const std::vector<int> &b = (A.size() > B.size()) ? A : B;
  double ratio = (b.size() + 0.0) / (a.size() + 0.0);
  return ratio > 9.0 ? listIntersectionBS(a, b) : listIntersection2P(a, b);
}

std::vector<int> ANDQuery(std::vector<std::string> &query, Index &index) {
  auto comp = [&](const std::string &a, const std::string &b) {
    return index[a]->size() < index[b]->size();
  };
  std::sort(query.begin(), query.end(), comp);

  std::vector<int> res = *index[query[0]];
  for (auto &word : query)
    if (word != query[0]) {
      res = listIntersection(res, *index[word]);
    }
  return res;
}

std::vector<int> listsUnion(int L, int R, std::vector<int> &index) {
  std::vector<int> res;
  for (int i = 0; i < (int)index.size(); i++)
    if (index[i] >= L && index[i] <= R)
      res.push_back(i);
  return res;
}

} // namespace boolean_model
