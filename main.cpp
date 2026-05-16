#include <iostream>
#include "memory_controller.hpp"
#include "ttl_index.hpp"
#include "storage.hpp"
#include "runner.hpp"
#include "command_executor.hpp"

int main(int argc, const char** argv) {
  MemoryController memctr;
  TTLIndex ttl_ind;

  Storage storage(memctr, ttl_ind);

  CommandExecutor executor(storage, memctr);

  Runner runner(std::cin, std::cout, std::cerr, executor);

  return runner.Run();
}