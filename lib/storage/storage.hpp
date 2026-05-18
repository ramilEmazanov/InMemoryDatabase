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
  std::expected<T*, StorageError> GetValueAs(const std::string& key) {
    ProcessExpired();
    if (storage_.find(key) != storage_.end()) {
      if (std::holds_alternative<T>(storage_[key].val_)) {
        return &std::get<T>(storage_[key].val_);
      }
      return std::unexpected{StorageError{WrongTypeRequested{}}};
    }
    return std::unexpected{StorageError{NoSuchKey{key}}};
  }

  template <typename T>
  std::expected<T*, StorageError> MutateValueAs(const std::string& key,
                                                auto&& callable) {
    auto result = GetValueAs<T>(key);
    if (!result.has_value()) {
      return std::unexpected{result.error()};
    }
    T* ptr = result.value();
    std::size_t old_size = CountValueMemoryUsage(storage_.at(key).val_);
    std::forward<decltype(callable)>(callable)(*ptr);
    std::size_t new_size = CountValueMemoryUsage(storage_.at(key).val_);
    memctr_.SubtractBytes(old_size);
    memctr_.AddBytes(new_size);
    return ptr;
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

  void SetKeyExpiration(const std::string& key, std::size_t seconds);

  long long GetKeyTTL(const std::string& key);

 private:
  std::unordered_map<std::string, Entry> storage_;
  MemoryController& memctr_;
  TimeController& timectr_;
};