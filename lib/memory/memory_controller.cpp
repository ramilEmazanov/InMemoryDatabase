#include "memory_controller.hpp"

#include <stdexcept>

MemoryController::MemoryController(std::size_t max_bytes)
    : used_bytes_(0), max_bytes_(max_bytes) {}

void MemoryController::AddBytes(std::size_t val) { used_bytes_ += val; }

void MemoryController::SubtractBytes(std::size_t val) {
  used_bytes_ = (val > used_bytes_) ? 0 : used_bytes_ - val;
}

std::size_t MemoryController::GetStorageMemoryUsage() const {
  return used_bytes_;
}

std::size_t MemoryController::GetMaxMemoryUsage() const { return max_bytes_; }

void MemoryController::SetLimitBytes(std::size_t max_bytes) {
  max_bytes_ = max_bytes;
}

bool MemoryController::WouldOverflow(std::size_t size) const {
  if (max_bytes_ == 0) {
    return false;
  }

  return used_bytes_ + size > max_bytes_;
}

void MemoryController::Reset() { used_bytes_ = 0; }
