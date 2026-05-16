#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <variant>

#include "memory_controller.hpp"
#include "ttl_index.hpp"
#include "types.hpp"

class Storage {
 public:
  Storage(MemoryController& memctr, TTLIndex& ttl_ind);

  template <typename T>
  T* GetValueAs(const std::string& key);

  void SetValue(std::string key, Value val);

 private:
  std::unordered_map<std::string, Entry> storage_;
  MemoryController memctr_;
  TTLIndex ttl_ind_;
};