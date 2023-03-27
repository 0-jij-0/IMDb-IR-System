#include "spelling_corrector.hpp"

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <vector>

SpellingCorrector::TrieNode::TrieNode() : next_node(alphabet_size, SIZE_MAX) {}

SpellingCorrector::QueryAlternative::QueryAlternative(
    const std::string &word, const std::vector<int> &matches)
    : word(word), matches(matches) {}

bool SpellingCorrector::QueryAlternative::operator<(
    const QueryAlternative &rhs) const {
  return matches.size() > rhs.matches.size();
}

SpellingCorrector::SpellingCorrector() = default;

SpellingCorrector::SpellingCorrector(const Index &index) : trie_(1) {
  if (character_map_.empty()) {
    int id = 0;
    character_map_['\''] = 36;
    for (char c = 'a'; c <= 'z'; c++) {
      character_map_[c] = id++;
    }
    for (char c = '0'; c <= '9'; c++) {
      character_map_[c] = id++;
    }
  }

  // Insert the index's words into the trie.
  for (const auto &[word, indices] : index) {
    TrieInsert(word);
  }
}

std::vector<int> SpellingCorrector::CorrectSpelling(const std::string &query,
                                                    const Index &index) {
  std::vector<QueryAlternative> top_query_alternatives =
      GetTopQueryAlternatives(query, index);

  if (top_query_alternatives.empty()) {
    std::cout << "\nQuery \"" << query << "\" was not recognized.\n";
    return std::vector<int>();
  }

  std::cout << "\nQuery \"" + query + "\": Did you mean:\n";
  int suggestions_count = 0;
  for (auto &x : top_query_alternatives) {
    std::cout << "[" << ++suggestions_count << "]: " << x.word << '\n';
  }
  std::cout << "[" << ++suggestions_count << "]: None\n";
  int chosen_option;
  std::cin >> chosen_option;

  return chosen_option > 0 || chosen_option < suggestions_count
             ? top_query_alternatives[chosen_option - 1].matches
             : std::vector<int>();
}

void SpellingCorrector::TrieInsert(const std::string &word) {
  // Do not insert words containing unsupported characters.
  for (char c : word) {
    if (!character_map_.count(c)) {
      return;
    }
  }

  size_t cur_node = 0;
  for (char c : word) {
    int nxt = character_map_[c];
    if (trie_[cur_node].next_node[nxt] == SIZE_MAX) {
      trie_[cur_node].next_node[nxt] = trie_.size();
      trie_.emplace_back();
    }
    cur_node = trie_[cur_node].next_node[nxt];
  }
  trie_[cur_node].is_word = true;
}

void SpellingCorrector::GetWordAlternatives(
    const std::string &input, size_t i, int threshold, std::string &word,
    int node, std::set<std::string> &alternatives) {
  if (threshold == 0) { // No more operations allowed.
    // Just advance until reaching the end of the word.
    size_t original_size = word.size();
    for (; i < input.size() && node != SIZE_MAX; ++i) {
      node = trie_[node].next_node[character_map_[input[i]]];
      word.push_back(input[i]);
    }
    if (node != SIZE_MAX && trie_[node].is_word) {
      alternatives.insert(word);
    }
    word.resize(original_size);
    return;
  }

  enum OperationType { kAdd = 0, kReplace = 1, kErase = 2, kDiscard = 3 };

  auto apply_operation = [&](OperationType operation_type) {
    bool skip_operation = false;
    switch (operation_type) {
    case kAdd: // Never skip an add operation.
      break;
    case kReplace:
    case kErase: // Make sure there's a character to replace/delete.
      skip_operation = i == input.size();
      break;
    case kDiscard: // Only discard in the end to avoid redundancy.
      skip_operation = i != input.size();
      break;
    }

    if (!skip_operation) {
      if (operation_type == kErase || operation_type == kDiscard) {
        return GetWordAlternatives(input, i + (operation_type == kErase),
                                   threshold - 1, word, node, alternatives);
      }
      for (const auto &[character, idx] : character_map_) {
        if (size_t next_node = trie_[node].next_node[idx];
            next_node != SIZE_MAX &&
            (operation_type == OperationType::kAdd || character != input[i])) {
          word.push_back(character);
          GetWordAlternatives(input, i + (operation_type == kReplace),
                              threshold - 1, word, next_node, alternatives);
          word.pop_back();
        }
      }
    }
  };

  // Apply threshold-decreasing operations.
  apply_operation(kDiscard);
  apply_operation(kAdd);
  apply_operation(kReplace);
  apply_operation(kErase);

  // Try using the available character if possible.
  if (i != input.size()) {
    if (size_t next_node = trie_[node].next_node[character_map_[input[i]]];
        next_node != SIZE_MAX) {
      word.push_back(input[i]);
      GetWordAlternatives(input, i + 1, threshold, word, next_node,
                          alternatives);
      word.pop_back();
    }
  }
}

void SpellingCorrector::GetQueryAlternatives(
    const std::vector<std::set<std::string>> &alternatives, size_t i,
    std::priority_queue<QueryAlternative> &topK_alternatives,
    const std::vector<int> &curList, const std::string &cur_query,
    const std::string &prev_word, const Index &index) {
  // The current generated sentence has no matches.
  if (curList.empty() ||
      (!topK_alternatives.empty() &&
       curList.size() < topK_alternatives.top().matches.size())) {
    return;
  }
  if (i == alternatives.size()) {
    // Keep track of the top 5 alternative queries. Quality is measured by the
    // number of matches a query has.
    if (topK_alternatives.size() < max_num_query_alternatives) {
      topK_alternatives.emplace(cur_query, curList);
      return;
    }
    if (curList.size() > topK_alternatives.top().matches.size()) {
      topK_alternatives.pop();
      topK_alternatives.emplace(cur_query, curList);
    }
    return;
  }

  for (auto &x : alternatives[i]) {
    if (index.count(prev_word + " " + x)) {
      GetQueryAlternatives(alternatives, i + 1, topK_alternatives,
                           boolean_model::ListIntersection(
                               curList, *index.at(prev_word + " " + x)),
                           cur_query + x + " ", x, index);
    }
  }
}

std::vector<SpellingCorrector::QueryAlternative>
SpellingCorrector::GetTopQueryAlternatives(const std::string &query,
                                           const Index &index) {
  // Generate alternatives for every separate word subject to edit distance
  // constraint.
  std::stringstream ss(query);
  std::vector<std::set<std::string>> word_alternatives;
  for (std::string word = "", temp = ""; ss >> word; temp = "") {
    word_alternatives.emplace_back();
    GetWordAlternatives(word, 0, edit_distance_threshold, temp, 0,
                        word_alternatives.back());
  }

  // Get the top K queries from combinations of the generated alternatives.
  std::priority_queue<QueryAlternative> topK_alternatives;
  for (auto &x : word_alternatives[0])
    GetQueryAlternatives(word_alternatives, 1, topK_alternatives, *index.at(x),
                         x + " ", x, index);

  // Get the final query_alternatives by order match count.
  std::vector<QueryAlternative> top_query_alternatives;
  while (!topK_alternatives.empty()) {
    top_query_alternatives.push_back(topK_alternatives.top());
    topK_alternatives.pop();
  }
  reverse(top_query_alternatives.begin(), top_query_alternatives.end());

  return top_query_alternatives;
}
