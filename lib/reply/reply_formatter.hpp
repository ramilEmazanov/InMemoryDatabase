#pragma once

#include "overload.hpp"
#include "reply.hpp"

class ReplyFormatter {
 public:
  static void Format(const Reply& reply, std::ostream& out);
};
