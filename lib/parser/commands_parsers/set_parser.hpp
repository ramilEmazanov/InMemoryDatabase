#pragma once

#include <expected>
#include <vector>

#include "command_parser.hpp"
#include "errors.hpp"
#include "set_commands.hpp"

std::expected<ParsedCommand, ParsingError> SAddCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> SRemCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> SIsMemberCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> SMembersCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> SCardCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> SUnionCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> SInterCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> SDiffCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> SMoveCommandParse(
    const std::vector<Token>& arguments);
