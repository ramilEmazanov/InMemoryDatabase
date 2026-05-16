#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <variant>
#include <iostream>

#include "memory_controller.hpp"
#include "ttl_index.hpp"
#include "types.hpp"

class Storage {
 public:
  Storage(MemoryController& memctr, TTLIndex& ttl_ind);

  template <typename T>
  T* GetValueAs(const std::string& key) {
    if (std::holds_alternative<T>(storage_[key].val_)) {
      T* ptr = &std::get<T>(storage_[key].val_);
      // std::cout << 2;
      // std::cout << *ptr << '\n';
      return ptr;
    } else {
      std::cout << 1;
      return nullptr;
    }
  }

  void SetValue(std::string key, Value val);

 private:
  std::unordered_map<std::string, Entry> storage_;
  MemoryController memctr_;
  TTLIndex ttl_ind_;
};