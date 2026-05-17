#pragma once

#include <expected>
#include <string>
#include <vector>

#include "errors.hpp"
#include "token.hpp"

class Tokenizer {
 public:
  static std::expected<std::vector<Token>, LexicalError> Tokenize(
      std::string_view line);  
};