#pragma once

#include <fstream>

class Runner {
 public:
  Runner(std::istream& in, std::ostream& out);

  void Run();

 private:
  std::istream& in_;
  std::ostream& out_;
};