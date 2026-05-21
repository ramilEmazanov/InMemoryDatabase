#pragma once
#include <expected>
#include <vector>

#include "command_parser.hpp"
#include "errors.hpp"
#include "string_commands.hpp"

std::expected<ParsedCommand, ParsingError> SetCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> GetCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> StrlenCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> AppendCommandParse(
    const std::vector<Token>& arguments);
