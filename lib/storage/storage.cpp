#include "storage.hpp"

#include <deque>
#include <string>
#include <unordered_map>
#include <variant>

#include "errors.hpp"
#include "memory_controller.hpp"
#include "time_controller.hpp"
#include "types.hpp"

Storage::Storage(MemoryController& memctr, TimeController& timectr)
    : memctr_(memctr), timectr_(timectr) {}

void Storage::ProcessExpired() {
  auto expired_keys = timectr_.CleanExpired();

  for (auto& key : expired_keys) {
    EraseValue(key);
  }
}

std::expected<void, StorageError> Storage::SetValue(const std::string& key,
                                                    Value&& val) {
  ProcessExpired();
  std::size_t size_bytes = CountValueMemoryUsage(val);

  if (storage_.find(key) != storage_.end()) {
    std::size_t size_val_curr = CountValueMemoryUsage(storage_.at(key).val_);

    if (!memctr_.CanReplace(size_val_curr, size_bytes)) {
      return std::unexpected{
          OutOfMemoryError{size_bytes, memctr_.GetStorageMemoryUsage()}};
    }

    memctr_.SubtractBytes(size_val_curr);

  } else {
    if (memctr_.WouldOverflow(size_bytes)) {
      return std::unexpected{
          OutOfMemoryError{size_bytes, memctr_.GetStorageMemoryUsage()}};
    }
  }

  storage_[key] = Entry{std::move(val), std::nullopt, 0};
  memctr_.AddBytes(size_bytes);
  return std::expected<void, StorageError>{};
}

bool Storage::EraseValue(const std::string& key) {
  if (storage_.find(key) != storage_.end()) {
    memctr_.SubtractBytes(CountValueMemoryUsage(storage_.at(key).val_));
    timectr_.DeleteExpiration(key);
    storage_.erase(key);
    return true;
  } else {
    return false;
  }
}

bool Storage::Exists(const std::string& key) const {
  if (storage_.find(key) != storage_.end()) {
    return true;
  } else {
    return false;
  }
}

std::string Storage::GetTypeName(const std::string& key) const {
  return std::string{};
}

std::size_t Storage::TotalKeyCount() const { return storage_.size(); }

std::vector<std::string> Storage::FindKeysMatching(std::string pattern) const {
  return std::vector<std::string>{};
}

void Storage::Flush() {
  memctr_.Reset();
  timectr_.Reset();
  storage_.clear();
}

std::size_t Storage::CountKeyMemoryUsage(const std::string& key) const {
  return CountValueMemoryUsage(storage_.at(key).val_);
}