#pragma once

#include <fstream>
#include "command_executor.hpp"

class Runner {
 public:
  Runner(std::istream& in, std::ostream& out, std::ostream& error_out, CommandExecutor& executor);

  int Run();

 private:
  std::istream& in_;
  std::ostream& out_;
  std::ostream& error_out_;
  CommandExecutor& executor_;
};