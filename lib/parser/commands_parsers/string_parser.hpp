#pragma once
#include <expected>
#include <span>

#include "command_parser.hpp"
#include "errors.hpp"
#include "parsed_command.hpp"

std::expected<ParsedCommand, ParsingError> SetCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> GetCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> StrlenCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> AppendCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> ExpireCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> TTLCommandParse(
    std::span<const Token> arguments);
