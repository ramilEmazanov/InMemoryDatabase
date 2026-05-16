#include "storage.hpp"

#include <deque>
#include <string>
#include <unordered_map>
#include <variant>

#include "memory_controller.hpp"
#include "ttl_index.hpp"
#include "types.hpp"

void Storage::SetValue(std::string key, Value val) {
  // auto entry = storage_[key];
  // entry.val_ = val;
  storage_[key] = Entry{val, std::nullopt, 0};
}

Storage::Storage(MemoryController& memctr, TTLIndex& ttl_ind)
    : memctr_(memctr), ttl_ind_(ttl_ind) {}
