#include <commands_table.hpp>
#include <control.hpp>
#include <parser.hpp>
#include <storage.hpp>
#include <types.hpp>
#include <iostream>

int main(int argc, const char** argv) {
  Storage storage;
  ParseCli(argc, argv, storage);

  CommandsTable commands_table;
  AddCommands(commands_table);

  Parser parser;

  std::string line;

  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }

    Command cmd = parser.Parse(line);

    if (cmd.IsExit()) {
      break;
    }
    
    Result result = commands_table.ProcessCommand(storage, cmd);
  }
}