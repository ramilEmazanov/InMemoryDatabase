#pragma once

#include <expected>
#include <string>
#include <vector>

#include "token.hpp"

class Tokenizer {
 public:
  static std::expected<std::vector<Token>, LexicalError> Tokenize(
      std::string_view line);

    static void foo();
};