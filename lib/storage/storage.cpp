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
  std::size_t size_bytes = CountValueMemoryUsage(val);
  ProcessExpired();

  if (storage_.find(key) != storage_.end()) {
    std::size_t size_val_curr = CountValueMemoryUsage(storage_.at(key).val_);
    std::size_t size_after_ = memctr_.GetStorageMemoryUsage() - size_val_curr;

    if (memctr_.GetMaxMemoryUsage() < size_after_ + size_bytes) {
      return std::unexpected{
          OutOfMemoryError{size_bytes, memctr_.GetStorageMemoryUsage()}};
    } else {
      memctr_.SubtractBytes(size_val_curr);
    }

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

std::expected<std::size_t, StorageError> Storage::GeoAddPoints(
    const std::string& key,
    const std::vector<std::pair<std::string, GeoPoint>>& points) {
  ProcessExpired();

  auto it = storage_.find(key);

  if (it != storage_.end()) {
    if (!std::holds_alternative<GeoValue>(it->second.val_)) {
      return std::unexpected{StorageError{WrongTypeRequested{}}};
    }

    GeoValue& geo = std::get<GeoValue>(it->second.val_);

    std::size_t new_count = 0;
    std::size_t delta = 0;
    for (const auto& [member, point] : points) {
      if (!geo.count(member)) {
        ++new_count;
        delta += sizeof(std::string) + member.size() + sizeof(GeoPoint);
      }
    }

    if (delta > 0 && memctr_.WouldOverflow(delta)) {
      return std::unexpected{StorageError{
          OutOfMemoryError{delta, memctr_.GetStorageMemoryUsage()}}};
    }

    for (const auto& [member, point] : points) {
      geo[member] = point;
    }

    memctr_.AddBytes(delta);
    return new_count;
  }

  GeoValue geo;
  for (const auto& [member, point] : points) {
    geo[member] = point;
  }

  std::size_t new_count = geo.size();
  std::size_t delta = sizeof(GeoValue);
  for (const auto& [member, point] : geo) {
    delta += sizeof(std::string) + member.size() + sizeof(GeoPoint);
  }

  if (memctr_.WouldOverflow(delta)) {
    return std::unexpected{
        StorageError{OutOfMemoryError{delta, memctr_.GetStorageMemoryUsage()}}};
  }

  storage_[key] = Entry{std::move(geo), std::nullopt, 0};
  memctr_.AddBytes(delta);
  return new_count;
}

std::string Storage::GetTypeName(const std::string& key) const {
  return std::string{};
}

std::size_t Storage::GetTotalKeyCount() const { return storage_.size(); }

std::vector<std::string> Storage::FindKeysMatching(std::string pattern) const {
  return std::vector<std::string>{};
}

void Storage::Flush() {
  timectr_.Reset();
  memctr_.Reset();
  storage_.clear();
}

std::size_t Storage::CountKeyMemoryUsage(const std::string& key) const {
  return CountValueMemoryUsage(storage_.at(key).val_);
}

void Storage::SetKeyExpiration(const std::string& key, std::size_t seconds) {
  timectr_.SetExpiration(key, seconds);
}

long long Storage::GetKeyTTL(const std::string& key) {
  ProcessExpired();
  if (storage_.find(key) == storage_.end()) {
    return -2;
  }
  return timectr_.GetRemainingSeconds(key);
}

std::size_t Storage::GetMaxMemory() const {
  return memctr_.GetMaxMemoryUsage();
}

void Storage::SetLimitMemory(std::size_t limit) {
  memctr_.SetLimitBytes(limit);
}