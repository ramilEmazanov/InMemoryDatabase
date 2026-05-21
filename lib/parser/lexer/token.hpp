#pragma once

#include <string>

enum class TokenKind { kBareword, kQuoted };

struct Token {
  TokenKind kind;
  std::string lexeme;
};