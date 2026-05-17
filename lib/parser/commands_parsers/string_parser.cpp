#include "string_parser.hpp"
#include "string_commands.hpp"

#include <expected>
#include <iostream>
#include <span>

#include "errors.hpp"

std::expected<ParsedCommand, ParsingError> SetCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 4) {
    return std::unexpected(WrongArity{"SET", 3, arguments.size() - 1});
  }

  return SetCommand{arguments[1].lexeme, arguments[2].lexeme};
}

std::expected<ParsedCommand, ParsingError> GetCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"GET", 2, arguments.size() - 1});
  }

  return GetCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> StrlenCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"STRLEN", 2, arguments.size() - 1});
  }

  return StrlenCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> AppendCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 4) {
    return std::unexpected(WrongArity{"APPEND", 3, arguments.size() - 1});
  }

  return AppendCommand{arguments[1].lexeme, arguments[2].lexeme};
}

