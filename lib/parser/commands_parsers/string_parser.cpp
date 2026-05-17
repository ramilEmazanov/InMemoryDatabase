#include "string_parser.hpp"

#include <expected>
#include <iostream>

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

std::expected<ParsedCommand, ParsingError> ExpireCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 4) {
    return std::unexpected(WrongArity{"EXPIRE", 3, arguments.size() - 1});
  }

  const std::string& seconds_token = arguments[2].lexeme;
  std::size_t seconds = 0;

  try {
    seconds = std::stoull(seconds_token);
  } catch (...) {
    return std::unexpected(BadInteger{seconds_token});
  }

  return ExpireCommand{arguments[1].lexeme, seconds};
}

std::expected<ParsedCommand, ParsingError> TTLCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"TTL", 2, arguments.size() - 1});
  }

  return TTLCommand{arguments[1].lexeme};
}