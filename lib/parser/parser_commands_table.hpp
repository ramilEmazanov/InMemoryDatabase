#pragma once

#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

#include "command_parser.hpp"
#include "errors.hpp"
#include "parsed_command.hpp"
#include "string_parser.hpp"

using ParseFunction = std::expected<ParsedCommand, ParsingError> (*)(
    const std::vector<Token>& arguments);

const std::unordered_map<std::string, ParseFunction> GetCommandTable();