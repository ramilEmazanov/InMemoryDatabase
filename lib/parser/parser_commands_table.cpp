#include <parser_commands_table.hpp>
#include <string_parser.hpp>

const std::unordered_map<std::string, ParseFunction> GetCommandTable() {
  std::unordered_map<std::string, ParseFunction> table;

  table.emplace("SET", static_cast<ParseFunction>(&SetCommandParse));
  // table.emplace("GET", &GetCommandParse);
  // table.emplace("STRLEN", &StrlenCommandParse);
  // table.emplace("APPEND", &AppendCommandParse);
  // table.emplace("EXPIRE", &AppendCommandParse);
  // table.emplace("TTL", &TTLCommandParse);

  return table;
}