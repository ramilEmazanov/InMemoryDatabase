#pragma once

#include <expected>
#include <variant>
#include <parsed_command.hpp>
#include <token.hpp>
#include <span>

enum class ParsingError {};

class CommandParser {
  static std::expected<ParsedCommand, ParsingError> Parse(std::span<const Token> token);
};