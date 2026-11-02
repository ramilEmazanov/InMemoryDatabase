#include "list_executor.hpp"

#include <algorithm>

#include "command_executor.hpp"
#include "errors.hpp"
#include "reply.hpp"
#include "types.hpp"

ListExecutor::ListExecutor(CommandExecutor& executor) : executor_(executor) {}

Reply ListExecutor::operator()(const RPush& cmd) {
  auto result = executor_.storage_.MutateValueAdd<ListValue>(
      cmd.key, cmd.strs, [&](ListValue& list) {
        for (const auto& s : cmd.strs) list.push_back(s);
      });
  if (!result.has_value()) {
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }
  return Reply{UnsignedIntReply{result.value()}};
}

Reply ListExecutor::operator()(const LPush& cmd) {
  auto result = executor_.storage_.MutateValueAdd<ListValue>(
      cmd.key, cmd.strs, [&](ListValue& list) {
        for (const auto& s : cmd.strs) list.push_front(s);
      });
  if (!result.has_value()) {
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }
  return Reply{UnsignedIntReply{result.value()}};
}

Reply ListExecutor::operator()(const LPop& cmd) {
  auto reply = executor_.storage_.GetValueAs<ListValue>(cmd.key);
  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{NilReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  std::size_t count = cmd.count.value_or(1);
  auto begin = reply.value()->begin();
  auto end = count > reply.value()->size() ? reply.value()->end() : begin + count;

  std::vector<std::string> response;
  while (begin != end) response.emplace_back(*begin++);

  auto mutate = executor_.storage_.MutateValueSubtr<ListValue>(
      cmd.key, response, [&](ListValue& list) {
        std::size_t n = std::min(count, list.size());
        for (std::size_t i = 0; i < n; ++i) list.pop_front();
      });
  if (!mutate.has_value()) {
    return Reply{ErrorReply{FormatStorageError(mutate.error())}};
  }

  if (!cmd.count.has_value()) {
    return Reply{BulkStringReply{std::move(response.front())}};
  }
  return Reply{ArrayReply{std::move(response)}};
}

Reply ListExecutor::operator()(const RPop& cmd) {
  auto reply = executor_.storage_.GetValueAs<ListValue>(cmd.key);
  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{NilReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  std::size_t count = cmd.count.value_or(1);
  auto begin = reply.value()->begin();
  auto end = reply.value()->end();
  begin = count > reply.value()->size() ? begin
                                        : begin + (reply.value()->size() - count);

  std::vector<std::string> response;
  while (begin != end) response.emplace_back(*begin++);

  auto mutate = executor_.storage_.MutateValueSubtr<ListValue>(
      cmd.key, response, [&](ListValue& list) {
        std::size_t n = std::min(count, list.size());
        for (std::size_t i = 0; i < n; ++i) list.pop_back();
      });
  if (!mutate.has_value()) {
    return Reply{ErrorReply{FormatStorageError(mutate.error())}};
  }

  if (!cmd.count.has_value()) {
    return Reply{BulkStringReply{std::move(response.front())}};
  }
  return Reply{ArrayReply{std::move(response)}};
}

Reply ListExecutor::operator()(const LLen& cmd) {
  auto reply = executor_.storage_.GetValueAs<ListValue>(cmd.key);
  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{UnsignedIntReply{0}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }
  return Reply{UnsignedIntReply{reply.value()->size()}};
}

Reply ListExecutor::operator()(const LRange& cmd) {
  auto reply = executor_.storage_.GetValueAs<ListValue>(cmd.key);
  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{ArrayReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  long long size = reply.value()->size();
  long long start = cmd.start < 0 ? cmd.start + size : cmd.start;
  long long stop = cmd.stop < 0 ? cmd.stop + size : cmd.stop;
  start = std::max(0LL, start);
  stop = std::min(stop, size - 1);

  if (start > stop) return Reply{ArrayReply{}};

  std::vector<std::string> result;
  auto it = reply.value()->begin() + start;
  for (long long i = start; i <= stop; ++i, ++it) result.push_back(*it);

  return Reply{ArrayReply{std::move(result)}};
}

Reply ListExecutor::operator()(const LIndex& cmd) {
  auto reply = executor_.storage_.GetValueAs<ListValue>(cmd.key);
  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{NilReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  long long size = reply.value()->size();
  long long ind = cmd.ind < 0 ? cmd.ind + size : cmd.ind;
  if (ind < 0 || ind >= size) return Reply{NilReply{}};

  return Reply{BulkStringReply{reply.value()->at(ind)}};
}

Reply ListExecutor::operator()(const LSet& cmd) {
  auto reply = executor_.storage_.GetValueAs<ListValue>(cmd.key);
  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{ErrorReply{"ERR no such key"}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  long long size = reply.value()->size();
  long long ind = cmd.ind < 0 ? cmd.ind + size : cmd.ind;
  if (ind < 0 || ind >= size) return Reply{ErrorReply{"ERR index out of range"}};

  std::size_t idx = static_cast<std::size_t>(ind);
  auto mutate = executor_.storage_.MutateValueInPlace<ListValue>(
      cmd.key, reply.value()->at(idx).size(), cmd.val.size(),
      [&](ListValue& list) { list[idx] = cmd.val; });
  if (!mutate.has_value()) {
    return Reply{ErrorReply{FormatStorageError(mutate.error())}};
  }
  return Reply{VoidReply{}};
}

Reply ListExecutor::operator()(const LInsert& cmd) {
  auto reply = executor_.storage_.GetValueAs<ListValue>(cmd.key);
  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{IntegerReply{0}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  auto& list = *reply.value();
  auto it = std::find(list.begin(), list.end(), cmd.pivot);
  if (it == list.end()) return Reply{IntegerReply{-1}};
  if (!cmd.before) ++it;

  auto mutate = executor_.storage_.MutateValueAdd<ListValue>(
      cmd.key, {cmd.value},
      [&](ListValue& lst) { lst.insert(it, cmd.value); });
  if (!mutate.has_value()) {
    return Reply{ErrorReply{FormatStorageError(mutate.error())}};
  }
  return Reply{IntegerReply{static_cast<long long>(mutate.value())}};
}
