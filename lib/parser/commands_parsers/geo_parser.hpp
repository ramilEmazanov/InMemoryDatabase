#pragma once

#include <expected>
#include <span>

#include "command_parser.hpp"
#include "errors.hpp"
#include "geo_commands.hpp"

std::expected<ParsedCommand, ParsingError> GeoAddCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> GeoPosCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> GeoDistCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> GeoSearchCommandParse(
    std::span<const Token> arguments);

std::expected<ParsedCommand, ParsingError> GeoSearchStoreCommandParse(
    std::span<const Token> arguments);
