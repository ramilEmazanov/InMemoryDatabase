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
      [&](UnterminatedString st) {
        return std::format("Unterminated string in column {} ", st.column);
      },
      [&](InvalidEscapeSequence st) {
        return std::format("Invalid Escape Sequence : {} at column {}",
                           st.character, st.column);
      },
      [&](UnexpectedControlCharacter st) {
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
      [&](const WrongArity& e) {
        return std::format("Wrong arity for {}: expected {}, got {}",
                           e.command_name, e.expected, e.actual);
      },
      [&](const UnknownCommand& e) {
        return std::format("Unknown command: {}", e.command_name);
      },
      [&](const BadInteger& e) {
        return std::format("Bad integer: {}", e.token);
      },
      [&](const BadDouble& e) { return std::format("Bad float: {}", e.token); },
      [&](const BadKeyword& e) {
        return std::format("Bad keyword: {}, expected {}", e.token, e.expected);
      });

  return std::visit(overload, err);
}

struct OutOfMemoryError {
  std::size_t requested_;
  std::size_t left_;
};

struct WrongTypeRequested {
  std::string requested_;
  std::string left_;
};

struct NoSuchKey {
  std::string key_;
};

using StorageError =
    std::variant<OutOfMemoryError, WrongTypeRequested, NoSuchKey>;

inline std::string FormatStorageError(const StorageError& err) {
  auto overload = Overload(
      [&](const OutOfMemoryError& e) {
        return std::format(
            "OOM command not allowed when used memory > 'maxmemory'");
      },
      [&](const WrongTypeRequested&) -> std::string {
        return "WRONGTYPE Operation against a key holding the wrong kind of "
               "value";
      },
      [&](const NoSuchKey& e) {
        return std::format("ERR no such key '{}'", e.key_);
      });
  return std::visit(overload, err);
}
