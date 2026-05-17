#pragma once

#include <chrono>
#include <deque>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

struct GeoSpatial {
  double lat;
  double lon;
};

using StringValue = std::string;
using ListValue = std::deque<std::string>;
using SetValue = std::unordered_set<std::string>;
using GeoValue = std::unordered_map<std::string, GeoSpatial>;

using Value = std::variant<StringValue, ListValue, SetValue, GeoValue>;

struct Entry {
  Value val_;
  std::optional<std::chrono::steady_clock::time_point> expire_at_;
  std::size_t size_bytes_;
};

std::size_t CountValueMemoryUsage(const Value& val);