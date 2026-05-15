#pragma once
#include <storage.hpp>
#include <string>
#include <vector>

#include "types.hpp"

void ParseCli(int argc, const char** argv, Storage& storage) {}

class Parser {
  public:

  Command Parse(const std::string& line);

  private:
};


