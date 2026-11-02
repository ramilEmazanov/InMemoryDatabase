#include "string_executor.hpp"

#include <variant>

#include "command_executor.hpp"
#include "errors.hpp"
#include "reply.hpp"
#include "reply_formatter.hpp"
#include "string_commands.hpp"
#include "types.hpp"

StringExecutor::StringExecutor(CommandExecutor& executor)
    : executor_(executor) {}

Reply StringExecutor::operator()(const SetCommand& cmd) {
  auto reply = executor_.storage_.GetValueAs<StringValue>(cmd.key);

  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      auto adding = executor_.storage_.MutateValueAdd<StringValue>(
          cmd.key, {cmd.value},
          [&](StringValue& val) { val.append(cmd.value); });

      if (!adding.has_value()) {
        return Reply{ErrorReply{FormatStorageError(adding.error())}};
      } else {
        return Reply{VoidReply{}};
      }
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  std::size_t old_bytes = reply.value()->size();
  std::size_t new_bytes = cmd.value.size();

  auto mutate = executor_.storage_.MutateValueInPlace<StringValue>(
      cmd.key, old_bytes, new_bytes,
      [&](StringValue& str) { str = cmd.value; });

  if (!mutate.has_value()) {
    return Reply{ErrorReply{FormatStorageError(mutate.error())}};
  }

  executor_.storage_.ClearExpiration(cmd.key);
  return Reply{VoidReply{}};
}

Reply StringExecutor::operator()(const GetCommand& get_command) {
  auto reply = executor_.storage_.GetValueAs<std::string>(get_command.key);

  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{NilReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  return Reply{StringReply{*reply.value()}};
}

Reply StringExecutor::operator()(const StrlenCommand& strlen_command) {
  auto reply = executor_.storage_.GetValueAs<std::string>(strlen_command.key);

  if (!reply.has_value()) {
    if (std::holds_alternative<NoSuchKey>(reply.error())) {
      return Reply{UnsignedIntReply{0}};
    }
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  return Reply{UnsignedIntReply{reply.value()->size()}};
}

Reply StringExecutor::operator()(const AppendCommand& cmd) {
  auto reply = executor_.storage_.GetValueAs<std::string>(cmd.key);

  if (!reply.has_value() && std::holds_alternative<NoSuchKey>(reply.error())) {
    auto adding = executor_.storage_.MutateValueAdd<StringValue>(
        cmd.key, {cmd.value}, [&](StringValue& val) { val.append(cmd.value); });

    if (!adding.has_value()) {
      return Reply{ErrorReply{FormatStorageError(reply.error())}};
    } else {
      return Reply{UnsignedIntReply{cmd.value.size()}};
    }
  } else if (!reply.has_value()) {
    return Reply{ErrorReply{FormatStorageError(reply.error())}};
  }

  std::string now = reply.has_value() ? *reply.value() : std::string{};
  std::string added = now + cmd.value;

  auto set_reply = executor_.storage_.MutateValueInPlace<StringValue>(
      cmd.key, now.size(), added.size(), [&](StringValue& s) { s = added; });

  if (!set_reply.has_value()) {
    return Reply{ErrorReply{FormatStorageError(set_reply.error())}};
  }

  return Reply{UnsignedIntReply{added.size()}};
}