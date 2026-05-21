#pragma once

#include "list_commands.hpp"
#include "reply.hpp"

class CommandExecutor;

class ListExecutor {
 public:
  ListExecutor(CommandExecutor& executor);

  Reply operator()(const RPush& cmd);
  Reply operator()(const LPush& cmd);
  Reply operator()(const LPop& cmd);
  Reply operator()(const RPop& cmd);
  Reply operator()(const LLen& cmd);
  Reply operator()(const LRange& cmd);
  Reply operator()(const LIndex& cmd);
  Reply operator()(const LSet& cmd);
  Reply operator()(const LInsert& cmd);

 private:
  CommandExecutor& executor_;
};
