#pragma once

#include <expected>
#include "parsed_command.hpp"
#include <span>
#include <token.hpp>
#include <variant>

enum class ParsingError {};

class CommandParser {
 public:
  static std::expected<ParsedCommand, ParsingError> Parse(
      std::span<const Token> token);
};