#include <iostream>
#include "memory_controller.hpp"
#include "time_controller.hpp"
#include "storage.hpp"
#include "runner.hpp"
#include "command_executor.hpp"

int main(int argc, const char** argv) {
  MemoryController memctr;
  TimeController timectrl;

  Storage storage(memctr, timectrl);

  CommandExecutor executor(storage, memctr);

  Runner runner(std::cin, std::cout, std::cerr, executor);

  return runner.Run();
}