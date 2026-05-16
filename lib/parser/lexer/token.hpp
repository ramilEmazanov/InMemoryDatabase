#pragma once

#include <string>

#include "errors.hpp"

enum class TokenKind { Bareword, Quoted, EndOfLine };

struct Token {
  TokenKind kind;
  std::string lexeme;
};