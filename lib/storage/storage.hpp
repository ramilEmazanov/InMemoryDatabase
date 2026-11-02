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
      if (std::holds_alternative<T>(storage_[key])) {
        return &std::get<T>(storage_[key]);
      }
      return std::unexpected{StorageError{WrongTypeRequested{}}};
    }
    return std::unexpected{StorageError{NoSuchKey{key}}};
  }

  template <ContainerType T, typename Func>
  std::expected<std::size_t, StorageError> MutateValueAdd(
      const std::string& key, const std::vector<std::string>& new_items,
      Func&& func) {
    ProcessExpired();

    std::size_t delta = 0;
    for (const auto& s : new_items) {
      delta += sizeof(std::string) + s.size();
    }

    auto it = storage_.find(key);

    if (it != storage_.end()) {
      if (!std::holds_alternative<T>(it->second)) {
        return std::unexpected{StorageError{WrongTypeRequested{}}};
      }
      if (memctr_.WouldOverflow(delta)) {
        return std::unexpected{StorageError{
            OutOfMemoryError{delta, memctr_.GetStorageMemoryUsage()}}};
      }
      T& container = std::get<T>(it->second);
      (func)(container);
      memctr_.AddBytes(delta);
      return container.size();
    }

    std::size_t total_delta = delta;
    if constexpr (!std::is_same_v<T, StringValue>) {
      total_delta += sizeof(T);
    }
    if (memctr_.WouldOverflow(total_delta)) {
      return std::unexpected{StorageError{
          OutOfMemoryError{total_delta, memctr_.GetStorageMemoryUsage()}}};
    }

    T container;
    (func)(container);
    storage_[key] = std::move(container);
    memctr_.AddBytes(total_delta);
    return std::get<T>(storage_.at(key)).size();
  }

  template <ContainerType T, typename Func>
  std::expected<std::size_t, StorageError> MutateValueSubtr(
      const std::string& key, const std::vector<std::string>& new_items,
      Func&& func) {
    ProcessExpired();

    std::size_t delta = 0;
    for (const auto& s : new_items) {
      delta += sizeof(std::string) + s.size();
    }

    auto it = storage_.find(key);

    if (it != storage_.end()) {
      if (!std::holds_alternative<T>(it->second)) {
        return std::unexpected{StorageError{WrongTypeRequested{}}};
      }

      T& container = std::get<T>(it->second);
      (func)(container);
      memctr_.SubtractBytes(delta);
      return container.size();
    }

    return delta;
  }

  template <ContainerType T, typename Func>
  std::expected<std::size_t, StorageError> MutateValueInPlace(
      const std::string& key, std::size_t old_bytes, std::size_t new_bytes,
      Func&& func) {
    ProcessExpired();

    auto it = storage_.find(key);

    if (it == storage_.end()) {
      return std::unexpected{StorageError{NoSuchKey{key}}};
    }

    if (!std::holds_alternative<T>(it->second)) {
      return std::unexpected{StorageError{WrongTypeRequested{}}};
    }

    if (new_bytes > old_bytes && memctr_.WouldOverflow(new_bytes - old_bytes)) {
      return std::unexpected{StorageError{OutOfMemoryError{
          new_bytes - old_bytes, memctr_.GetStorageMemoryUsage()}}};
    }

    T& container = std::get<T>(it->second);
    (func)(container);

    memctr_.SubtractBytes(old_bytes);
    memctr_.AddBytes(new_bytes);

    return container.size();
  }

  std::expected<std::size_t, StorageError> GeoAddPoints(
      const std::string& key,
      const std::vector<std::pair<std::string, GeoPoint>>& points);

  std::expected<void, StorageError> PutValue(const std::string& key,
                                             Value&& val);

  bool EraseValue(const std::string& key);

  bool Exists(const std::string& key);

  std::string GetTypeName(const std::string& key) const;

  std::size_t GetTotalKeyCount() const;

  std::vector<std::string> FindKeysMatching(std::string pattern);

  void Flush();

  std::size_t CountKeyMemoryUsage(const std::string& key) const;

  void ProcessExpired();

  void SetKeyExpiration(const std::string& key, std::size_t seconds);

  void ClearExpiration(const std::string& key);

  long long GetKeyTTL(const std::string& key);

  std::size_t GetMaxMemory() const;

  void SetLimitMemory(std::size_t limit);

 private:
  std::unordered_map<std::string, Value> storage_;
  MemoryController& memctr_;
  TimeController& timectr_;
};