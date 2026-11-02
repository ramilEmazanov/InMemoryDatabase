#pragma once

#include <expected>
#include <token.hpp>
#include <variant>
#include <vector>

#include "errors.hpp"
#include "parsed_command.hpp"

class CommandParser {
 public:
  static std::expected<ParsedCommand, ParsingError> Parse(
      const std::vector<Token>& token);
};