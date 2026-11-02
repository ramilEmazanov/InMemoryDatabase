#include "reply_formatter.hpp"

#include <cstddef>
#include <iostream>
#include <variant>

#include "overload.hpp"
#include "reply.hpp"

void ReplyFormatter::Format(const Reply& reply, std::ostream& out) {
  auto overload = Overload(
    [&](const VoidReply&) {
      out << "OK\n";
    },
    [&](const NilReply&) {
      out << "(nil)\n";
    },
    [&](const IntegerReply& r) {
      out << "(integer) " << r.num << '\n';
    },
    [&](const UnsignedIntReply& r) {
      out << "(integer) " << r.num << '\n';
    },
    [&](const StringReply& r) {
      out << '"' << r.value << '"' << '\n';
    },
    [&](const ArrayReply& r) {
      for (std::size_t i = 0; i < r.arr.size(); ++i) {
        out << (i + 1) << ") \"" << r.arr[i] << "\"\n";
      }
      if (r.arr.empty()) {
        out << "(empty array)\n";
      }
    },
    [&](const ErrorReply& e) {
      out << "(error) " << e.message << '\n';
    },
    [&](const BulkStringReply& e) {
      out << e.message << '\n';
    },
    [&](const GeoPosReply& r) {
      for (std::size_t i = 0; i < r.entries.size(); ++i) {
        out << (i + 1) << ") ";
        if (r.entries[i].has_value()) {
          out << "1) \"" << r.entries[i]->first << "\"\n";
          out << "   2) \"" << r.entries[i]->second << "\"\n";
        } else {
          out << "(nil)\n";
        }
      }
      if (r.entries.empty()) {
        out << "(empty array)\n";
      }
    }
  );

  std::visit(overload, reply);
}
