#include "set_parser.hpp"

#include <expected>
#include <span>

#include "errors.hpp"
#include "set_commands.hpp"

std::expected<ParsedCommand, ParsingError> SAddCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 4) {
    return std::unexpected(WrongArity{"SADD", 3, arguments.size() - 1});
  }

  std::vector<std::string> members;
  members.reserve(arguments.size() - 2);

  for (std::size_t i = 2; i + 1 < arguments.size(); ++i) {
    members.push_back(arguments[i].lexeme);
  }

  return SAdd{arguments[1].lexeme, std::move(members)};
}

std::expected<ParsedCommand, ParsingError> SRemCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 4) {
    return std::unexpected(WrongArity{"SREM", 3, arguments.size() - 1});
  }

  std::vector<std::string> members;
  members.reserve(arguments.size() - 2);

  for (std::size_t i = 2; i + 1 < arguments.size(); ++i) {
    members.push_back(arguments[i].lexeme);
  }

  return SRem{arguments[1].lexeme, std::move(members)};
}

std::expected<ParsedCommand, ParsingError> SIsMemberCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 4) {
    return std::unexpected(WrongArity{"SISMEMBER", 3, arguments.size() - 1});
  }

  return SIsMember{arguments[1].lexeme, arguments[2].lexeme};
}

std::expected<ParsedCommand, ParsingError> SMembersCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"SMEMBERS", 2, arguments.size() - 1});
  }

  return SMembers{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> SCardCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 3) {
    return std::unexpected(WrongArity{"SCARD", 2, arguments.size() - 1});
  }

  return SCard{arguments[1].lexeme};
}

std::expected<ParsedCommand, ParsingError> SUnionCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 3) {
    return std::unexpected(WrongArity{"SUNION", 2, arguments.size() - 1});
  }

  std::vector<std::string> keys;
  keys.reserve(arguments.size() - 2);

  for (std::size_t i = 1; i + 1 < arguments.size(); ++i) {
    keys.push_back(arguments[i].lexeme);
  }

  return SUnion{std::move(keys)};
}

std::expected<ParsedCommand, ParsingError> SInterCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 3) {
    return std::unexpected(WrongArity{"SINTER", 2, arguments.size() - 1});
  }

  std::vector<std::string> keys;
  keys.reserve(arguments.size() - 2);
  for (std::size_t i = 1; i + 1 < arguments.size(); ++i) {
    keys.push_back(arguments[i].lexeme);
  }

  return SInter{std::move(keys)};
}

std::expected<ParsedCommand, ParsingError> SDiffCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 3) {
    return std::unexpected(WrongArity{"SDIFF", 2, arguments.size() - 1});
  }

  std::vector<std::string> keys;
  keys.reserve(arguments.size() - 2);
  for (std::size_t i = 1; i + 1 < arguments.size(); ++i) {
    keys.push_back(arguments[i].lexeme);
  }

  return SDiff{std::move(keys)};
}

std::expected<ParsedCommand, ParsingError> SMoveCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 5) {
    return std::unexpected(WrongArity{"SMOVE", 4, arguments.size() - 1});
  }

  return SMove{arguments[1].lexeme, arguments[2].lexeme, arguments[3].lexeme};
}
