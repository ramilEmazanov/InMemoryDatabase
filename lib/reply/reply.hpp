#pragma once

#include <string>
#include <variant>
#include <vector>

struct VoidReply {};

struct NilReply {};

struct IntegerReply {
  long long num;
};

struct UnsignedIntReply {
  unsigned long long num;
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

using Reply = std::variant<VoidReply, NilReply, UnsignedIntReply, IntegerReply,
                           StringReply, ArrayReply, ErrorReply>;