#pragma once

#include <deque>
#include <expected>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>

#include "errors.hpp"
#include "memory_controller.hpp"
#include "time_controller.hpp"
#include "types.hpp"

class Storage {
 public:
  Storage(MemoryController& memctr, TimeController& timectr);

  template <typename T>
  T* GetValueAs(const std::string& key) {
    if (std::holds_alternative<T>(storage_[key].val_)) {
      T* ptr = &std::get<T>(storage_[key].val_);
      return ptr;
    } else {
      return nullptr;
    }
  }

  std::expected<void, StorageError> SetValue(const std::string& key,
                                             Value&& val);

  bool EraseValue(const std::string& key);

  bool Exists(const std::string& key) const;

  std::string GetTypeName(const std::string& key) const;

  std::size_t TotalKeyCount() const;

  std::vector<std::string> FindKeysMatching(std::string pattern) const;

  void Flush();

  std::size_t CountKeyMemoryUsage(const std::string& key) const;

  void ProcessExpired();

 private:
  std::unordered_map<std::string, Entry> storage_;
  MemoryController& memctr_;
  TimeController& timectr_;
};