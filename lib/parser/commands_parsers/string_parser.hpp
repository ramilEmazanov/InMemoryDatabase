#pragma once
#include <expected>
#include <span>

#include "command_parser.hpp"
#include "parsed_command.hpp"

std::expected<ParsedCommand, ParsingError> SetCommandParse(
    std::span<const Token> arguments);

void GetCommandParse();

void StrlenCommandParse();

void AppendCommandParse();

void ExpireCommandParse();

void TTLCommandParse();
