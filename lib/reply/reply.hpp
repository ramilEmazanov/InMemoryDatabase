#pragma once

#include <string>
#include <variant>
#include <vector>

struct VoidReply {};

struct IntegerReply {
  long long num;
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
    std::variant<VoidReply, IntegerReply, StringReply, ArrayReply, ErrorReply>;