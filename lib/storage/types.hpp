#pragma once

#include <chrono>
#include <deque>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <variant>
#include <vector>

using String = std::string;
using List = std::deque<std::string>;
using Set = std::set<std::string>;
using Geo = std::map<std::string, std::pair<double, double>>;

struct Entry {
  std::variant<String, List, Set, Geo> entry_;
  std::optional<std::chrono::steady_clock::time_point> expire_;
  std::size_t size_bytes_;
};

struct Result {
  std::string str;
};

class Command {
 public:
 private:
  std::string name_;
  std::vector<std::string> args_;
};