#include <algorithm>
#include <sstream>
#include <string>
#include <tokenizer.hpp>
#include <types.hpp>
#include <unordered_map>
#include <vector>

std::expected<std::vector<Token>, LexicalError> Tokenize(
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
}
