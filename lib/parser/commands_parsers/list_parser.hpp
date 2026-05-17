#pragma once
#include <expected>
#include <span>

#include "command_parser.hpp"
#include "errors.hpp"
#include "list_commands.hpp"

std::expected<ParsedCommand, ParsingError> LPushCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> RPushCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> LPopCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> RPopCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> LLenCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> LRangeCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> LIndexCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> LSetCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> LInsertCommandParse(
    std::span<const Token> arguments);
