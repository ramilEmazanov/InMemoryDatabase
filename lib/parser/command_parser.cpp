#include <command_parser.hpp>
#include <parser_commands_table.hpp>

std::expected<ParsedCommand, ParsingError> CommandParser::Parse(
    std::span<const Token> token) {
  auto func_map = GetCommandTable();

  std::string first_token = token[0].lexeme;

  auto result = func_map[first_token];

  return result(token);
}
