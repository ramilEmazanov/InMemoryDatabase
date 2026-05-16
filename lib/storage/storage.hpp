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
  template<typename T>
  T* GetValueAs(const std::string& key);

  void SetValue(std::string key, Value val);

 private:
  std::unordered_map<std::string, Entry> storage_;
  MemoryController memctr_;
  TTLIndex ttl_ind_;
};