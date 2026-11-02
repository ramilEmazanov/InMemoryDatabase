#pragma once

#include <expected>
#include <vector>

#include "command_parser.hpp"
#include "errors.hpp"
#include "general_commands.hpp"

std::expected<ParsedCommand, ParsingError> ExpireCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> TTLCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> TypeCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> DelCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> ExistsCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> KeysCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> FlushDbCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> ConfigCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> DbSizeCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> MemoryCommandParse(
    const std::vector<Token>& arguments);
