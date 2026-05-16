#pragma once

#include <expected>
#include <span>
#include <token.hpp>
#include <variant>

#include "parsed_command.hpp"
#include "errors.hpp"

class CommandParser {
 public:
  static std::expected<ParsedCommand, ParsingError> Parse(
      std::span<const Token> token);
};