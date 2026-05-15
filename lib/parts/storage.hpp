#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <variant>

#include "types.hpp"

class Storage {
 public:
  void get();

 private:
  std::unordered_map<std::string, Entry> storage_;
  size_t maxmemory_;
  size_t used_;
};