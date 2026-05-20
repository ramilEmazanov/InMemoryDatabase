#pragma once

#include <optional>
#include <string>
#include <vector>

struct LPush {
  std::string key;
  std::vector<std::string> strs;
};

struct RPush {
  std::string key;
  std::vector<std::string> strs;
};

struct LPop {
  std::string key;
  std::optional<std::size_t> count;
};

struct RPop {
  std::string key;
  std::optional<std::size_t> count;
};

struct LLen {
  std::string key;
};

struct LRange {
  std::string key;
  int start;
  int stop;
};

struct LIndex {
  std::string key;
  long long ind;
};

struct LSet {
  std::string key;
  long long ind;
  std::string val;
};

struct LInsert {
  std::string key;
  bool before;
  std::string pivot;
  std::string value;
};