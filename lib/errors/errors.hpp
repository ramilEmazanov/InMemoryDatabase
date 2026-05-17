#pragma once

#include <format>
#include <string>
#include <variant>

#include "overload.hpp"

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

using LexicalError = std::variant<UnterminatedString, InvalidEscapeSequence,
                                  UnexpectedControlCharacter>;

inline std::string FormatLexicalError(const LexicalError& err) {
  auto overload = Overload(
      [&](UnterminatedString st) -> std::string {
        return std::format("Unterminated string in column {} ", st.column);
      },
      [&](InvalidEscapeSequence st) -> std::string {
        return std::format("Invalid Escape Sequence : {} at column {}",
                           st.character, st.column);
      },
      [&](UnexpectedControlCharacter st) -> std::string {
        return std::format("Unexpected Control Character : {} at column {}",
                           st.character, st.column);
      });

  return std::visit(overload, err);
}

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

using ParsingError =
    std::variant<WrongArity, UnknownCommand, BadInteger, BadFloat, BadKeyword>;

struct OutOfMemoryError {};
