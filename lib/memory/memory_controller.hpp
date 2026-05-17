#pragma once

#include <cstddef>

class MemoryController {
 public:
  void AddBytes(std::size_t val);

  void SubtractBytes(std::size_t val);

  std::size_t GetStorageMemoryUsage() const;

  std::size_t GetMaxMemoryUsage() const;

  void SetLimitBytes();

  bool WouldOverflow(std::size_t size) const;

 private:
  std::size_t used_bytes_;
  std::size_t max_bytes_;
};