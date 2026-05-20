#include "geo_parser.hpp"

#include <algorithm>
#include <expected>
#include <span>
#include <string>

#include "errors.hpp"
#include "geo_commands.hpp"

namespace {

std::expected<double, ParsingError> ParseDouble(const std::string& token) {
  try {
    std::size_t pos = 0;
    double val = std::stod(token, &pos);
    if (pos != token.size()) {
      return std::unexpected(BadDouble{token});
    }
    return val;
  } catch (...) {
    return std::unexpected(BadDouble{token});
  }
}

std::string ToUpper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

bool IsValidUnit(const std::string& unit) {
  return unit == "m" || unit == "km" || unit == "mi" || unit == "ft";
}

struct GeoSearchOptions {
  std::optional<bool> sort_asc;
  std::optional<std::size_t> count;
};

std::expected<GeoSearchOptions, ParsingError> ParseGeoSearchOptions(
    std::span<const Token> tokens) {
  GeoSearchOptions opts;
  std::size_t i = 0;

  while (i < tokens.size()) {
    std::string kw = ToUpper(tokens[i].lexeme);

    if (kw == "ASC") {
      opts.sort_asc = true;
      ++i;
    } else if (kw == "DESC") {
      opts.sort_asc = false;
      ++i;
    } else if (kw == "COUNT") {
      if (i + 1 >= tokens.size()) {
        return std::unexpected(WrongArity{"COUNT", 2, 1});
      }
      try {
        opts.count = std::stoull(tokens[i + 1].lexeme);
      } catch (...) {
        return std::unexpected(BadInteger{tokens[i + 1].lexeme});
      }
      i += 2;
    } else {
      return std::unexpected(BadKeyword{tokens[i].lexeme, "ASC|DESC|COUNT"});
    }
  }

  return opts;
}

}  // namespace

std::expected<ParsedCommand, ParsingError> GeoAddCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 6 || (arguments.size() - 3) % 3 != 0) {
    return std::unexpected(WrongArity{"GEOADD", 5, arguments.size() - 1});
  }

  GeoAdd cmd;
  cmd.key = arguments[1].lexeme;

  for (std::size_t i = 2; i + 1 < arguments.size(); i += 3) {
    auto lon = ParseDouble(arguments[i].lexeme);
    if (!lon.has_value()) {
      return std::unexpected(lon.error());
    }
    auto lat = ParseDouble(arguments[i + 1].lexeme);
    if (!lat.has_value()) {
      return std::unexpected(lat.error());
    }
    cmd.points.push_back({lon.value(), lat.value(), arguments[i + 2].lexeme});
  }

  return cmd;
}

std::expected<ParsedCommand, ParsingError> GeoPosCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 4) {
    return std::unexpected(WrongArity{"GEOPOS", 3, arguments.size() - 1});
  }

  GeoPos cmd;
  cmd.key = arguments[1].lexeme;
  cmd.members.reserve(arguments.size() - 3);

  for (std::size_t i = 2; i + 1 < arguments.size(); ++i) {
    cmd.members.push_back(arguments[i].lexeme);
  }

  return cmd;
}

std::expected<ParsedCommand, ParsingError> GeoDistCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() != 5 && arguments.size() != 6) {
    return std::unexpected(WrongArity{"GEODIST", 4, arguments.size() - 1});
  }

  std::string unit = "m";

  if (arguments.size() == 6) {
    unit = arguments[4].lexeme;
    if (!IsValidUnit(unit)) {
      return std::unexpected(BadKeyword{unit, "m|km|mi|ft"});
    }
  }

  return GeoDist{arguments[1].lexeme, arguments[2].lexeme, arguments[3].lexeme,
                 unit};
}

std::expected<ParsedCommand, ParsingError> GeoSearchCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 9) {
    return std::unexpected(WrongArity{"GEOSEARCH", 8, arguments.size() - 1});
  }

  if (ToUpper(arguments[2].lexeme) != "FROMLONLAT") {
    return std::unexpected(BadKeyword{arguments[2].lexeme, "FROMLONLAT"});
  }

  auto lon = ParseDouble(arguments[3].lexeme);
  if (!lon.has_value()) {
    return std::unexpected(lon.error());
  }
  auto lat = ParseDouble(arguments[4].lexeme);
  if (!lat.has_value()) {
    return std::unexpected(lat.error());
  }

  if (ToUpper(arguments[5].lexeme) != "BYRADIUS") {
    return std::unexpected(BadKeyword{arguments[5].lexeme, "BYRADIUS"});
  }

  auto radius = ParseDouble(arguments[6].lexeme);
  if (!radius.has_value()) {
    return std::unexpected(radius.error());
  }

  std::string unit = arguments[7].lexeme;
  if (!IsValidUnit(unit)) {
    return std::unexpected(BadKeyword{unit, "m|km|mi|ft"});
  }

  auto opts_result =
      ParseGeoSearchOptions(arguments.subspan(8, arguments.size() - 9));
  if (!opts_result.has_value()) {
    return std::unexpected(opts_result.error());
  }

  return GeoSearch{arguments[1].lexeme, lon.value(), lat.value(),
                   radius.value(),      unit,        opts_result->sort_asc,
                   opts_result->count};
}

std::expected<ParsedCommand, ParsingError> GeoSearchStoreCommandParse(
    std::span<const Token> arguments) {
  if (arguments.size() < 10) {
    return std::unexpected(
        WrongArity{"GEOSEARCHSTORE", 9, arguments.size() - 1});
  }

  if (ToUpper(arguments[3].lexeme) != "FROMLONLAT") {
    return std::unexpected(BadKeyword{arguments[3].lexeme, "FROMLONLAT"});
  }

  auto lon = ParseDouble(arguments[4].lexeme);
  if (!lon.has_value()) {
    return std::unexpected(lon.error());
  }

  auto lat = ParseDouble(arguments[5].lexeme);
  if (!lat.has_value()) {
    return std::unexpected(lat.error());
  }

  if (ToUpper(arguments[6].lexeme) != "BYRADIUS") {
    return std::unexpected(BadKeyword{arguments[6].lexeme, "BYRADIUS"});
  }

  auto radius = ParseDouble(arguments[7].lexeme);
  if (!radius.has_value()) {
    return std::unexpected(radius.error());
  }

  std::string unit = arguments[8].lexeme;
  if (!IsValidUnit(unit)) {
    return std::unexpected(BadKeyword{unit, "m|km|mi|ft"});
  }

  auto opts_result =
      ParseGeoSearchOptions(arguments.subspan(9, arguments.size() - 10));
  if (!opts_result) return std::unexpected(opts_result.error());

  return GeoSearchStore{arguments[1].lexeme,   arguments[2].lexeme, lon.value(),
                        lat.value(),           radius.value(),      unit,
                        opts_result->sort_asc, opts_result->count};
}
