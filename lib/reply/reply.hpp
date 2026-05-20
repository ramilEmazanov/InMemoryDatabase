#pragma once

#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

struct VoidReply {};

struct NilReply {};

struct IntegerReply {
  long long num;
};

struct BulkStringReply {
  std::string message;
};

struct UnsignedIntReply {
  std::size_t num;
};

struct StringReply {
  std::string value;
};

struct ArrayReply {
  std::vector<std::string> arr;
};

struct ErrorReply {
  std::string message;
};

struct GeoPosReply {
  std::vector<std::optional<std::pair<double, double>>> entries;
};

using Reply = std::variant<VoidReply, BulkStringReply, NilReply,
                           UnsignedIntReply, IntegerReply, StringReply,
                           ArrayReply, ErrorReply, GeoPosReply>;