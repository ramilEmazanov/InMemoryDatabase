#pragma once

#include "memory_controller.hpp"
#include "parsed_command.hpp"
#include "reply.hpp"
#include "storage.hpp"

class CommandExecutor {
 public:
  CommandExecutor(Storage& storage, MemoryController& memctr)
      : storage_(storage), memctr_(memctr) {}

  Reply Execute(const ParsedCommand& command);

 private:
  Storage& storage_;
  MemoryController& memctr_;
};