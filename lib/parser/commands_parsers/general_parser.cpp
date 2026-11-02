#include "general_parser.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <expected>
#include <string>

#include "errors.hpp"

std::expected<ParsedCommand, ParsingError> ExpireCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"EXPIRE", 3, arguments.size()});
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
    const std::vector<Token>& arguments) {
  if (arguments.size() != 2) {
    return std::unexpected(WrongArity{"TTL", 2, arguments.size()});
  }

  return TTLCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> TypeCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 2) {
    return std::unexpected(WrongArity{"TYPE", 2, arguments.size()});
  }

  return TypeCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> DelCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() < 2) {
    return std::unexpected(WrongArity{"DEL", 2, arguments.size()});
  }

  std::vector<std::string> keys;
  keys.reserve(arguments.size() - 1);

  for (std::size_t i = 1; i < arguments.size(); ++i) {
    keys.push_back(arguments[i].lexeme);
  }

  return DelCommand{std::move(keys)};
}

std::expected<ParsedCommand, ParsingError> ExistsCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() < 2) {
    return std::unexpected(WrongArity{"EXISTS", 2, arguments.size()});
  }

  std::vector<std::string> keys;
  keys.reserve(arguments.size() - 1);

  for (std::size_t i = 1; i < arguments.size(); ++i) {
    keys.push_back(arguments[i].lexeme);
  }

  return ExistsCommand{std::move(keys)};
}

std::expected<ParsedCommand, ParsingError> KeysCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 2) {
    return std::unexpected(WrongArity{"KEYS", 2, arguments.size()});
  }

  return KeysCommand{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> FlushDbCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 1) {
    return std::unexpected(WrongArity{"FLUSHDB", 1, arguments.size()});
  }

  return FlushDbCommand{};
}

std::expected<ParsedCommand, ParsingError> ConfigCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() < 3) {
    return std::unexpected(WrongArity{"CONFIG", 3, arguments.size()});
  }

  std::string subcommand = arguments[1].lexeme;
  std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(),
                 ::toupper);

  if (subcommand == "SET") {
    if (arguments.size() != 4) {
      return std::unexpected(WrongArity{"CONFIG SET", 4, arguments.size()});
    }
    const std::string& param = arguments[2].lexeme;
    const std::string& val_token = arguments[3].lexeme;
    std::size_t value = 0;
    try {
      value = std::stoull(val_token);
    } catch (...) {
      return std::unexpected(BadInteger{val_token});
    }
    return ConfigSetCommand{param, value};
  }

  if (subcommand == "GET") {
    if (arguments.size() != 3) {
      return std::unexpected(WrongArity{"CONFIG GET", 3, arguments.size()});
    }
    return ConfigGetCommand{arguments[2].lexeme};
  }

  return std::unexpected(BadKeyword{arguments[1].lexeme, "SET|GET"});
}

std::expected<ParsedCommand, ParsingError> DbSizeCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() != 1) {
    return std::unexpected(WrongArity{"DBSIZE", 1, arguments.size()});
  }

  return DbSizeCommand{};
}

std::expected<ParsedCommand, ParsingError> MemoryCommandParse(
    const std::vector<Token>& arguments) {
  if (arguments.size() < 2) {
    return std::unexpected(WrongArity{"MEMORY", 2, arguments.size()});
  }

  std::string subcommand = arguments[1].lexeme;
  std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(),
                 ::toupper);

  if (subcommand != "USAGE") {
    return std::unexpected(BadKeyword{arguments[1].lexeme, "USAGE"});
  }

  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"MEMORY USAGE", 3, arguments.size()});
  }

  return MemoryUsageCommand{arguments[2].lexeme};
}
