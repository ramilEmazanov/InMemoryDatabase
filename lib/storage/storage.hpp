#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <variant>

#include "types.hpp"
#include "memory_controller.hpp"
#include "ttl_index.hpp"

class Storage {
 public:
  void get();

 private:
  std::unordered_map<std::string, Entry> storage_;
  MemoryController memctr_;
  TTLIndex ttl_ind_;
};