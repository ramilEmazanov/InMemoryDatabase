#pragma once

#include "reply.hpp"
#include "set_commands.hpp"

class CommandExecutor;

class SetExecutor {
 public:
  SetExecutor(CommandExecutor& executor);

  Reply operator()(const SAdd& cmd);
  Reply operator()(const SRem& cmd);
  Reply operator()(const SIsMember& cmd);
  Reply operator()(const SMembers& cmd);
  Reply operator()(const SCard& cmd);
  Reply operator()(const SUnion& cmd);
  Reply operator()(const SInter& cmd);
  Reply operator()(const SDiff& cmd);
  Reply operator()(const SMove& cmd);

 private:
  CommandExecutor& executor_;
};
