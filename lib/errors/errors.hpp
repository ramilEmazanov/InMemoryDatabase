#pragma once

#include <string>
#include <variant>

struct UnterminatedString {
  std::size_t column;
};

struct InvalidEscapeSequence {
  std::size_t column;
  char character;
};

struct UnexpectedControlCharacter {
  std::size_t column;
  char character;
};

using LexicalError = std::variant<UnterminatedString, InvalidEscapeSequence, UnexpectedControlCharacter>;

struct WrongArity {
  std::string command_name;
  std::size_t expected;
  std::size_t actual;
};

struct UnknownCommand {
  std::string command_name;
};

struct BadInteger {
  std::string token;
};

struct BadFloat {
  std::string token;
};

struct BadKeyword {
  std::string token;
  std::string expected;
};

using ParsingError = std::variant<WrongArity, UnknownCommand, BadInteger, BadFloat, BadKeyword>;

struct OutOfMemoryError {};
