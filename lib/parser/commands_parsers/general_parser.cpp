#include <expected>
#include <iostream>

#include "errors.hpp"
#include "string_parser.hpp"
#include "general_parser.hpp"

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

std::expected<ParsedCommand, ParsingError> TypeCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> DelCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> ExistsCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> KeysCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> FlushDbCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> ConfigCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> DbSizeCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> MemoryCommandParse(
    std::span<const Token> arguments) {}