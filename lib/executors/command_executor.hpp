#pragma once

#include "memory_controller.hpp"
#include "parsed_command.hpp"
#include "reply.hpp"
#include "storage.hpp"

class StringExecutor;
class ListExecutor;
class SetExecutor;
class GeoExecutor;
class GeneralExecutor;

class CommandExecutor {
 public:
  CommandExecutor(Storage& storage, MemoryController& memctr)
      : storage_(storage), memctr_(memctr) {}

  Reply Execute(const ParsedCommand& command);

  friend StringExecutor;
  friend ListExecutor;
  friend SetExecutor;
  friend GeoExecutor;
  friend GeneralExecutor;

 private:
  Storage& storage_;
  MemoryController& memctr_;
};
