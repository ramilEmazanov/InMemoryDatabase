#include "tokenizer.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "errors.hpp"

std::expected<std::vector<Token>, LexicalError> Tokenizer::Tokenize(
    std::string_view line) {
  std::vector<Token> res;
  std::size_t column = 0;

  while (column < line.size()) {
    while (line[column] == ' ' || line[column] == '\t') {
      column++;
    }

    if (column == line.size()) {
      break;
    }

    std::size_t start = column;

    while (line[column] != ' ' && line[column] != '\t') {
      column++;
    }

    std::string token(line.substr(start, column));
    res.push_back(Token{TokenKind::Bareword, token});
  }
  res.push_back(Token{TokenKind::EndOfLine, ""});
  return res;
}