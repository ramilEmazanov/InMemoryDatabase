#pragma once

#include "reply.hpp"
#include "string_commands.hpp"

class CommandExecutor;

class StringExecutor {
 public:
  StringExecutor(CommandExecutor& executor);

  Reply operator()(const SetCommand& cmd);
  Reply operator()(const AppendCommand& cmd);
  Reply operator()(const StrlenCommand& strlen_command);
  Reply operator()(const GetCommand& get_command);

 private:
  CommandExecutor& executor_;
};