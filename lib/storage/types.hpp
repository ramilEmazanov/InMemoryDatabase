#pragma once

#include <deque>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

struct GeoPoint {
  double lat;
  double lon;
};

using StringValue = std::string;
using ListValue = std::deque<std::string>;
using SetValue = std::unordered_set<std::string>;
using GeoValue = std::map<std::string, GeoPoint>;

using Value = std::variant<StringValue, ListValue, SetValue, GeoValue>;

std::size_t CountValueMemoryUsage(const Value& val);

template <typename T>
concept ContainerType =
    std::is_same_v<T, StringValue> || std::is_same_v<T, ListValue> ||
    std::is_same_v<T, SetValue> || std::is_same_v<T, GeoValue>;