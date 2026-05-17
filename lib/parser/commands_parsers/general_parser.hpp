#pragma once

#include <expected>
#include <span>

#include "command_parser.hpp"
#include "errors.hpp"
#include "general_commands.hpp"

std::expected<ParsedCommand, ParsingError> ExpireCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> TTLCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> TypeCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> DelCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> ExistsCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> KeysCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> FlushDbCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> ConfigCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> DbSizeCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> MemoryCommandParse(
    std::span<const Token> arguments);
