#pragma once

#include <expected>
#include <vector>

#include "command_parser.hpp"
#include "errors.hpp"
#include "geo_commands.hpp"

std::expected<ParsedCommand, ParsingError> GeoAddCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> GeoPosCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> GeoDistCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> GeoSearchCommandParse(
    const std::vector<Token>& arguments);

std::expected<ParsedCommand, ParsingError> GeoSearchStoreCommandParse(
    const std::vector<Token>& arguments);
