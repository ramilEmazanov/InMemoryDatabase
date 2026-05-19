#include "list_parser.hpp"

#include <algorithm>
#include <expected>
#include <span>

#include "errors.hpp"
#include "list_commands.hpp"

std::expected<ParsedCommand, ParsingError> LPushCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 4) {
    return std::unexpected(WrongArity{"LPUSH", 3, arguments.size() - 1});
  }

  std::vector<std::string> values;
  values.reserve(arguments.size() - 2);

  for (std::size_t i = 2; i + 1 < arguments.size(); ++i) {
    values.push_back(arguments[i].lexeme);
  }

  return LPush{arguments[1].lexeme, std::move(values)};
}

std::expected<ParsedCommand, ParsingError> RPushCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 4) {
    return std::unexpected(WrongArity{"RPUSH", 3, arguments.size() - 1});
  }

  std::vector<std::string> values;
  values.reserve(arguments.size() - 2);

  for (std::size_t i = 2; i + 1 < arguments.size(); ++i) {
    values.push_back(arguments[i].lexeme);
  }

  return RPush{arguments[1].lexeme, std::move(values)};
}

std::expected<ParsedCommand, ParsingError> LPopCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3 && arguments.size() != 4) {
    return std::unexpected(WrongArity{"LPOP", 2, arguments.size() - 1});
  }

  if (arguments.size() == 3) {
    return LPop{arguments[1].lexeme, std::nullopt};
  }

  long long count = 0;
  try {
    count = std::stoll(arguments[2].lexeme);
  } catch (...) {
    return std::unexpected(BadInteger{arguments[2].lexeme});
  }

  return LPop{arguments[1].lexeme, static_cast<std::size_t>(count)};
}

std::expected<ParsedCommand, ParsingError> RPopCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3 && arguments.size() != 4) {
    return std::unexpected(WrongArity{"RPOP", 2, arguments.size() - 1});
  }

  if (arguments.size() == 3) {
    return RPop{arguments[1].lexeme, std::nullopt};
  }

  long long count = 0;
  try {
    count = std::stoll(arguments[2].lexeme);
  } catch (...) {
    return std::unexpected(BadInteger{arguments[2].lexeme});
  }

  return RPop{arguments[1].lexeme, static_cast<std::size_t>(count)};
}

std::expected<ParsedCommand, ParsingError> LLenCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"LLEN", 2, arguments.size() - 1});
  }

  return LLen{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> LRangeCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 5) {
    return std::unexpected(WrongArity{"LRANGE", 4, arguments.size() - 1});
  }

  int start = 0;
  int stop = 0;

  try {
    start = std::stoi(arguments[2].lexeme);
  } catch (...) {
    return std::unexpected(BadInteger{arguments[2].lexeme});
  }

  try {
    stop = std::stoi(arguments[3].lexeme);
  } catch (...) {
    return std::unexpected(BadInteger{arguments[3].lexeme});
  }

  return LRange{arguments[1].lexeme, start, stop};
}

std::expected<ParsedCommand, ParsingError> LIndexCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 4) {
    return std::unexpected(WrongArity{"LINDEX", 3, arguments.size() - 1});
  }

  long long idx = 0;
  try {
    idx = std::stoll(arguments[2].lexeme);
  } catch (...) {
    return std::unexpected(BadInteger{arguments[2].lexeme});
  }

  return LIndex{arguments[1].lexeme, static_cast<std::size_t>(idx)};
}

std::expected<ParsedCommand, ParsingError> LSetCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 5) {
    return std::unexpected(WrongArity{"LSET", 4, arguments.size() - 1});
  }

  long long idx = 0;
  try {
    idx = std::stoll(arguments[2].lexeme);
  } catch (...) {
    return std::unexpected(BadInteger{arguments[2].lexeme});
  }

  return LSet{arguments[1].lexeme, static_cast<std::size_t>(idx),
              arguments[3].lexeme};
}

std::expected<ParsedCommand, ParsingError> LInsertCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 6) {
    return std::unexpected(WrongArity{"LINSERT", 5, arguments.size() - 1});
  }

  std::string position = arguments[2].lexeme;
  std::transform(position.begin(), position.end(), position.begin(), ::toupper);

  bool before = false;
  if (position == "BEFORE") {
    before = true;
  } else if (position == "AFTER") {
    before = false;
  } else {
    return std::unexpected(BadKeyword{arguments[2].lexeme, "BEFORE|AFTER"});
  }

  return LInsert{arguments[1].lexeme, before, arguments[3].lexeme,
                 arguments[4].lexeme};
}
