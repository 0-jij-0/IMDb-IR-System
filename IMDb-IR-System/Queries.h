#pragma once

#include "SpellingCorrection.h"
#include "boolean_model.h"
#include "indexing.h"

std::vector<int> phraseQuery(const std::string &query, Index &index,
                             int INDEX_TYPE) {
  std::vector<std::string> words;
  std::stringstream ss(query);
  std::string word;
  while (ss >> word) {
    words.push_back(word);
  }
  if ((int)words.size() == 1) {
    return index.count(words[0]) ? *index[words[0]]
                                 : spellingCorrection(query, INDEX_TYPE);
  }

  if (!index.count(words[0] + " " + words[1]))
    return spellingCorrection(query, INDEX_TYPE);

  std::vector<int> res = *(index[words[0] + " " + words[1]]);
  std::string prev = words[1];
  for (int i = 2; i < (int)words.size(); i++) {
    std::string &cur = words[i];
    res = boolean_model::listIntersection(res, *index[prev + " " + cur]);
    prev = cur;
  }
  return res.empty() ? spellingCorrection(query, INDEX_TYPE) : res;
}

std::vector<int> nameQuery(const std::string &name) {
  return phraseQuery(name, nameIndex, NAME_INDEX);
}

std::vector<int> actorsQuery(const std::vector<std::string> &actors) {
  std::vector<int> res = phraseQuery(actors[0], actorIndex, ACTOR_INDEX);
  for (auto &actor : actors)
    if (actor != actors[0])
      res = boolean_model::listIntersection(
          res, phraseQuery(actor, actorIndex, ACTOR_INDEX));

  return res;
}

std::vector<int> characterQuery(const std::vector<std::string> &characters) {
  std::vector<int> res =
      phraseQuery(characters[0], characterIndex, CHARACTER_INDEX);
  for (auto &character : characters)
    if (character != characters[0])
      res = boolean_model::listIntersection(
          res, phraseQuery(character, characterIndex, CHARACTER_INDEX));

  return res;
}

std::vector<int> quoteQuery(const std::string &quote) {
  return phraseQuery(quote, quoteIndex, QUOTE_INDEX);
}

std::vector<int> yearQuery(int L, int R) {
  return boolean_model::listsUnion(L, R, showYear);
}

std::vector<int> ratingQuery(int L, int R) {
  return boolean_model::listsUnion(L, R, showRating);
}
