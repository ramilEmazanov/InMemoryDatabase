#include "general_parser.hpp"

#include <algorithm>
#include <expected>
#include <iostream>

#include "errors.hpp"
#include "string_parser.hpp"

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
    std::span<const Token> arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"TYPE", 2, arguments.size() - 1});
  }

  return TypeCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> DelCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 3) {
    return std::unexpected(WrongArity{"DEL", 2, arguments.size() - 1});
  }

  std::vector<std::string> keys;
  keys.reserve(arguments.size() - 2);

  for (std::size_t i = 1; i + 1 < arguments.size(); ++i) {
    keys.push_back(arguments[i].lexeme);
  }

  return DelCommand{std::move(keys)};
}

std::expected<ParsedCommand, ParsingError> ExistsCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 3) {
    return std::unexpected(WrongArity{"EXISTS", 2, arguments.size() - 1});
  }

  std::vector<std::string> keys;
  keys.reserve(arguments.size() - 2);

  for (std::size_t i = 1; i + 1 < arguments.size(); ++i) {
    keys.push_back(arguments[i].lexeme);
  }

  return ExistsCommand{std::move(keys)};
}

std::expected<ParsedCommand, ParsingError> KeysCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"KEYS", 2, arguments.size() - 1});
  }

  return KeysCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> FlushDbCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 2) {
    return std::unexpected(WrongArity{"FLUSHDB", 1, arguments.size() - 1});
  }

  return FlushDbCommand{};
}

std::expected<ParsedCommand, ParsingError> ConfigCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 4) {
    return std::unexpected(WrongArity{"CONFIG", 3, arguments.size() - 1});
  }

  std::string subcommand = arguments[1].lexeme;
  std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(),
                 ::toupper);

  if (subcommand == "SET") {
    if (arguments.size() != 5) {
      return std::unexpected(WrongArity{"CONFIG SET", 4, arguments.size() - 1});
    }
    return ConfigSetCommand{arguments[3].lexeme};
  }

  if (subcommand == "GET") {
    if (arguments.size() != 4) {
      return std::unexpected(WrongArity{"CONFIG GET", 3, arguments.size() - 1});
    }
    return ConfigGetCommand{arguments[2].lexeme};
  }

  return std::unexpected(BadKeyword{arguments[1].lexeme, "SET|GET"});
}

std::expected<ParsedCommand, ParsingError> DbSizeCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 2) {
    return std::unexpected(WrongArity{"DBSIZE", 1, arguments.size() - 1});
  }

  return DbSizeCommand{};
}

std::expected<ParsedCommand, ParsingError> MemoryCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 3) {
    return std::unexpected(WrongArity{"MEMORY", 2, arguments.size() - 1});
  }

  std::string subcommand = arguments[1].lexeme;
  std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(),
                 ::toupper);

  if (subcommand != "USAGE") {
    return std::unexpected(BadKeyword{arguments[1].lexeme, "USAGE"});
  }

  if (arguments.size() != 4) {
    return std::unexpected(WrongArity{"MEMORY USAGE", 3, arguments.size() - 1});
  }

  return MemoryUsageCommand{arguments[2].lexeme};
}