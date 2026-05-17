#include "memory_controller.hpp"

#include <cstddef>

void MemoryController::AddBytes(std::size_t val) {}

void MemoryController::SubtractBytes(std::size_t val) {}

std::size_t MemoryController::GetStorageMemoryUsage() const {}

std::size_t MemoryController::GetMaxMemoryUsage() const { return max_bytes_; }

void MemoryController::SetLimitBytes() {}

bool MemoryController::WouldOverflow(std::size_t size) const {}