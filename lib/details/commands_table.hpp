#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "storage.hpp"
#include "types.hpp"

using Args = std::vector<std::string>;
using Func = std::function<Result(Storage&, Args&)>;

class CommandDef {
 public:
  CommandDef(std::string name, int max_args, int min_args, Func func)
      : name_(name), max_args_(max_args), min_args_(min_args) {}

 private:
  std::string name_;
  int max_args_;
  int min_args_;
  Func func_;
};

class CommandsTable {
 public:
  void Add(CommandDef cmd);

  Result ProcessCommand(Storage& s, const Command& cmd);

 private:
  std::unordered_map<std::string, CommandDef> commands_;
};