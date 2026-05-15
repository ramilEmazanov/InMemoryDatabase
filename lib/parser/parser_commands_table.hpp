#pragma once

#include <expected>
#include <span>
#include <string>
#include <unordered_map>

#include "command_parser.hpp"
#include "parsed_command.hpp"
#include "string_parser.hpp"

using ParseFunction = std::expected<ParsedCommand, ParsingError> (*)(
    std::span<const Token> arguments);

const std::unordered_map<std::string, ParseFunction> GetCommandTable();