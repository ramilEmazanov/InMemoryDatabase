#pragma once
#include <expected>
#include <vector>

#include "command_parser.hpp"
#include "errors.hpp"
#include "list_commands.hpp"

std::expected<ParsedCommand, ParsingError> LPushCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> RPushCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> LPopCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> RPopCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> LLenCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> LRangeCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> LIndexCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> LSetCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> LInsertCommandParse(
    const std::vector<Token>& arguments);
