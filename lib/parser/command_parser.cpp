#include <command_parser.hpp>
#include <parser_commands_table.hpp>
#include "errors.hpp"

std::expected<ParsedCommand, ParsingError> CommandParser::Parse(
    std::span<const Token> token) {
  auto func_map = GetCommandTable();

  std::string first_token = token[0].lexeme;

  auto command_function = func_map[first_token];
  
  auto parse_result = command_function(token);

  return parse_result;
}
