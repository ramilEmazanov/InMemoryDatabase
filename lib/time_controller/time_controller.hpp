#pragma once

#include <chrono>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class TimeController {
 public:
  void SetExpiration(const std::string& key, std::size_t sec);

  std::vector<std::string> CleanExpired();

  void DeleteExpiration(const std::string& key);

  long long GetRemainingSeconds(const std::string& key) const;

  void Reset();

 private:
  std::multimap<std::chrono::steady_clock::time_point, std::string> by_time_;
  std::unordered_map<std::string, std::chrono::steady_clock::time_point>
      by_key_;
};
