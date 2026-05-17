#include <expected>
#include <span>

#include "errors.hpp"
#include "set_commands.hpp"
#include "set_parser.hpp"

std::expected<ParsedCommand, ParsingError> SAddCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> SRemCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> SIsMemberCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> SMembersCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> SCardCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> SUnionCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> SInterCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> SDiffCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> SMoveCommandParse(
    std::span<const Token> arguments) {}
