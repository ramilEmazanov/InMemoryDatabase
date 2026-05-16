#include "string_parser.hpp"

#include <expected>

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
    return std::unexpected(WrongArity{"SET", 2, arguments.size() - 1});
  }

  return GetCommand{arguments[1].lexeme};
}