#include <algorithm>
#include <command_parser.hpp>
#include <parser_commands_table.hpp>

#include "errors.hpp"

std::expected<ParsedCommand, ParsingError> CommandParser::Parse(
    std::span<const Token> token) {
  static auto func_map = GetCommandTable();

  std::string command_name = token[0].lexeme;
  std::string upper_name = command_name;
  std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(),
                 ::toupper);

  if (func_map.contains(upper_name)) {
    auto command_function = func_map[upper_name];
    auto parse_result = command_function(token);

    return parse_result;
  } else {
    return std::unexpected{UnknownCommand{command_name}};
  }
}
