#include <expected>
#include <span>

#include "errors.hpp"
#include "geo_commands.hpp"
#include "geo_parser.hpp"

std::expected<ParsedCommand, ParsingError> GeoAddCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> GeoPosCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> GeoDistCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> GeoSearchCommandParse(
    std::span<const Token> arguments) {}

std::expected<ParsedCommand, ParsingError> GeoSearchStoreCommandParse(
    std::span<const Token> arguments) {}
