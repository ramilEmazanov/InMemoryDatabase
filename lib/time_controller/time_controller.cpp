#include "time_controller.hpp"

using namespace std::chrono;

void TimeController::SetExpiration(const std::string& key, std::size_t sec) {
  steady_clock::time_point now = steady_clock::now();
  steady_clock::time_point expire_at = now + seconds(sec);

  DeleteExpiration(key);

  by_time_.emplace(expire_at, key);
  by_key_.emplace(key, expire_at);
}

std::vector<std::string> TimeController::CleanExpired() {
  std::vector<std::string> ans;

  auto now = steady_clock::now();

  auto bound = by_time_.upper_bound(now);

  auto it = by_time_.begin();

  while (it != bound) {
    ans.push_back(it->second);
    by_key_.erase(it->second);
    it = by_time_.erase(it);
  }

  return ans;
}

void TimeController::DeleteExpiration(const std::string& key) {
  if (by_key_.find(key) == by_key_.end()) {
    return;
  }

  auto expire_at = by_key_[key];
  by_key_.erase(key);

  auto [begin, end] = by_time_.equal_range(expire_at);

  while (begin != end) {
    if (begin->second == key) {
      by_time_.erase(begin);
      break;
    }
    begin++;
  }
}

void TimeController::Reset() {
  by_key_.clear();
  by_time_.clear();
}
