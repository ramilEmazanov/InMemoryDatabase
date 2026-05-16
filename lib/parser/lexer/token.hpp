#pragma once

#include <string>

enum class TokenKind { Bareword, Quoted, EndOfLine };

struct Token {
  TokenKind kind;
  std::string lexeme;
};