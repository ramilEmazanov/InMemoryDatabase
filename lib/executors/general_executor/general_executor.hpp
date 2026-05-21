#pragma once

#include "general_commands.hpp"
#include "reply.hpp"

class CommandExecutor;

class GeneralExecutor {
 public:
  GeneralExecutor(CommandExecutor& executor);

  Reply operator()(const ExpireCommand& cmd);
  Reply operator()(const TTLCommand& cmd);
  Reply operator()(const TypeCommand& cmd);
  Reply operator()(const DelCommand& cmd);
  Reply operator()(const ExistsCommand& cmd);
  Reply operator()(const KeysCommand& cmd);
  Reply operator()(const FlushDbCommand& cmd);
  Reply operator()(const ConfigSetCommand& cmd);
  Reply operator()(const ConfigGetCommand& cmd);
  Reply operator()(const DbSizeCommand& cmd);
  Reply operator()(const MemoryUsageCommand& cmd);

 private:
  CommandExecutor& executor_;
};
