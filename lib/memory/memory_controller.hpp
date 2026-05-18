#pragma once

#include <cstddef>

class MemoryController {
 public:
  MemoryController(std::size_t max_bytes = 0);

  void AddBytes(std::size_t val);

  void SubtractBytes(std::size_t val);

  std::size_t GetStorageMemoryUsage() const;

  std::size_t GetMaxMemoryUsage() const;

  void SetLimitBytes(std::size_t max_bytes);

  bool WouldOverflow(std::size_t size) const;

  void Reset();

  bool CanReplace(std::size_t curr, std::size_t adding) const;

 private:
  std::size_t used_bytes_;
  std::size_t max_bytes_;
};