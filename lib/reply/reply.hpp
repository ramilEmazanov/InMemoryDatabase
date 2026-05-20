#pragma once

#include <string>
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

using Reply =
    std::variant<VoidReply, BulkStringReply, NilReply, UnsignedIntReply,
                 IntegerReply, StringReply, ArrayReply, ErrorReply>;