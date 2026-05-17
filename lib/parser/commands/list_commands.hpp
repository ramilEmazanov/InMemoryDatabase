#pragma once

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
  std::size_t count;
};

struct RPop {
  std::string key;
  std::size_t count;
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
  std::size_t ind;
};

struct LSet {
  std::string key;
  std::size_t ind;
  std::string val;
};

struct LInsert {
  std::string key;
  bool before;
  std::string pivot;
  std::string value;
};