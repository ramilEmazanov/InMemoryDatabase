#include "reply_formatter.hpp"

void ReplyFormatter::Format(const Reply& reply) {
  auto overload = Overload(
    [](const VoidReply& void_reply) {},
    [](const IntegerReply& integer_reply) {},
    [](const StringReply& strig_reply) {},
    [](const ArrayReply& array_reply) {},
    [](const ErrorReply& error) {}
  );

  std::visit(overload, reply);
}
