#include "string_parser.hpp"
#include <expected>

std::expected<ParsedCommand, ParsingError> SetCommandParse(
    std::span<const Token> arguments) {}