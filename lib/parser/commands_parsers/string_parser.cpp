#include "string_parser.hpp"

#include <expected>

#include "errors.hpp"
#include "string_commands.hpp"

std::expected<ParsedCommand, ParsingError> SetCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"SET", 3, arguments.size()});
  }

  return SetCommand{arguments[1].lexeme, arguments[2].lexeme};
}

std::expected<ParsedCommand, ParsingError> GetCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 2) {
    return std::unexpected(WrongArity{"GET", 2, arguments.size()});
  }

  return GetCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> StrlenCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 2) {
    return std::unexpected(WrongArity{"STRLEN", 2, arguments.size()});
  }

  return StrlenCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> AppendCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"APPEND", 3, arguments.size()});
  }

  return AppendCommand{arguments[1].lexeme, arguments[2].lexeme};
}
