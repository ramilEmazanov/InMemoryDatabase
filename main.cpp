#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "command_executor.hpp"
#include "memory_controller.hpp"
#include "runner.hpp"
#include "storage.hpp"
#include "time_controller.hpp"

namespace {

std::size_t ParseMaxMemory(const std::string& input) {
  if (input.empty()) {
    throw std::invalid_argument("maxmemory value is empty");
  }

  std::string lower(input);
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  std::size_t multiplier = 1;
  std::string digits = lower;

  auto ends_with = [&](const std::string& suffix) {
    return lower.size() >= suffix.size() &&
           lower.compare(lower.size() - suffix.size(), suffix.size(), suffix) ==
               0;
  };

  if (ends_with("kb")) {
    multiplier = 1024;
    digits = input.substr(0, input.size() - 2);
  } else if (ends_with("mb")) {
    multiplier = 1024 * 1024;
    digits = input.substr(0, input.size() - 2);
  } else if (ends_with("gb")) {
    multiplier = 1024 * 1024 * 1024;
    digits = input.substr(0, input.size() - 2);
  } else if (ends_with("b")) {
    multiplier = 1;
    digits = input.substr(0, input.size() - 1);
  } else if (std::isalpha(input.back())) {
    throw std::invalid_argument("wrong maxmemory suffix");
  }

  if (digits.empty()) {
    throw std::invalid_argument("maxmemory value has no number");
  }

  std::size_t parsed_pos = 0;
  unsigned long long value = std::stoull(digits, &parsed_pos);

  if (parsed_pos != digits.size()) {
    throw std::invalid_argument("maxmemory value contains invalid characters");
  }

  if (value > std::numeric_limits<std::size_t>::max() / multiplier) {
    throw std::out_of_range("maxmemory value is too large");
  }

  return value * multiplier;
}

std::size_t ParseArgs(int argc, const char** argv) {
  std::size_t max_bytes = 0;

  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--maxmemory") == 0) {
      if (i + 1 >= argc) {
        throw std::invalid_argument("--maxmemory requires a value");
      }

      max_bytes = ParseMaxMemory(argv[i + 1]);
      ++i;
    } else {
      throw std::invalid_argument(std::format("unknown argument: {}", argv[i]));
    }
  }

  return max_bytes;
}

}  // namespace

int main(int argc, const char** argv) {
  std::size_t max_bytes = 0;

  try {
    max_bytes = ParseArgs(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << std::format("Invalid command line arguments: {}\n",
                             err.what());
    std::cerr << "Usage: ./valkey [--maxmemory <bytes>]\n";
    return 1;
  }

  MemoryController memctr(max_bytes);
  TimeController timectrl;

  Storage storage(memctr, timectrl);
  CommandExecutor executor(storage, memctr);
  Runner runner(std::cin, std::cout, std::cerr, executor);

  return runner.Run();
}