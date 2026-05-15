#pragma once

#include "parsed_command.hpp"
#include "reply.hpp"
#include "storage.hpp"
#include "memory_controller.hpp"

class CommandExecutor {
 public:
  Reply Execute(const ParsedCommand& command);

 private:
 Storage& storage_;
 MemoryController& memctr_;
};