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

struct BadDouble {
  std::string token;
};

struct BadKeyword {
  std::string token;
  std::string expected;
};

using ParsingError =
    std::variant<WrongArity, UnknownCommand, BadInteger, BadDouble, BadKeyword>;

inline std::string FormatParsingError(const ParsingError& err) {
  auto overload = Overload(
      [&](const WrongArity& e) -> std::string {
        return std::format("Wrong arity for {}: expected {}, got {}",
                           e.command_name, e.expected, e.actual);
      },
      [&](const UnknownCommand& e) -> std::string {
        return std::format("Unknown command: {}", e.command_name);
      },
      [&](const BadInteger& e) -> std::string {
        return std::format("Bad integer: {}", e.token);
      },
      [&](const BadDouble& e) -> std::string {
        return std::format("Bad float: {}", e.token);
      },
      [&](const BadKeyword& e) -> std::string {
        return std::format("Bad keyword: {}, expected {}", e.token, e.expected);
      });

  return std::visit(overload, err);
}

struct OutOfMemoryError {
  std::size_t requested_;
  std::size_t left_;
};

using StorageError = std::variant<OutOfMemoryError>;