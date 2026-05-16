#include "reply_formatter.hpp"
#include "overload.hpp"
#include "reply.hpp"
#include <variant>
#include <iostream>

void ReplyFormatter::Format(const Reply& reply, std::ostream& out) {
  auto overload = Overload(
    [&](const VoidReply& void_reply) {
      out << "void_reply\n";
    },
    [&](const IntegerReply& integer_reply) {
      out << "integer_reply\n";
    },
    [&](const StringReply& strig_reply) {
      out << "string_reply\n";

    },
    [&](const ArrayReply& array_reply) {
      out << "array_reply\n";
    },
    [&](const ErrorReply& error) {
      out << "error_reply\n";
    }
  );

  std::visit(overload, reply);
}
