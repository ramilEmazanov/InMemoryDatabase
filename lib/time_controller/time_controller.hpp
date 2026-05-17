#pragma once

#include <chrono>
#include <map>
#include <string>
#include <unordered_map>

class TimeController {
 public:
  void SetExpiration(const std::string& key, std::size_t sec);

  void CleanExpired();

  void CancelExpiration(const std::string& key);

 private:
  std::multimap<std::chrono::steady_clock::time_point, std::string> by_time_;
  std::unordered_map<std::string, std::chrono::steady_clock::time_point>
      by_key_;
};
