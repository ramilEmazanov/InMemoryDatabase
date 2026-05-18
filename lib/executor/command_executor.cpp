#include "command_executor.hpp"

#include <variant>

#include "errors.hpp"
#include "geo_commands.hpp"
#include "list_commands.hpp"
#include "overload.hpp"
#include "reply.hpp"
#include "set_commands.hpp"
#include "storage.hpp"

Reply CommandExecutor::Execute(const ParsedCommand& command) {
  auto overload = Overload{
      [&](const SetCommand& set_command) {
        auto reply =
            storage_.SetValue(set_command.key, StringValue{set_command.value});

        if (!reply.has_value()) {
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        return Reply{VoidReply{}};
      },
      [&](const GetCommand& get_command) {
        auto reply = storage_.GetValueAs<std::string>(get_command.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        return Reply{StringReply{*reply.value()}};
      },
      [&](const StrlenCommand& strlen_command) {
        auto reply = storage_.GetValueAs<std::string>(strlen_command.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{UnsignedIntReply{0}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        return Reply{UnsignedIntReply{reply.value()->size()}};
      },
      [&](const AppendCommand& append_command) {
        auto reply = storage_.GetValueAs<std::string>(append_command.key);

        if (!reply.has_value() &&
            !std::holds_alternative<NoSuchKey>(reply.error())) {
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::string now = reply.has_value() ? *reply.value() : std::string{};
        std::string added = now + append_command.value;

        auto set_reply =
            storage_.SetValue(append_command.key, StringValue{added});

        if (!set_reply.has_value()) {
          return Reply{ErrorReply{FormatStorageError(set_reply.error())}};
        }

        return Reply{UnsignedIntReply{added.size()}};
      },

      [&](const RPush& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<WrongTypeRequested>(reply.error())) {
            return Reply{ErrorReply{FormatStorageError(reply.error())}};
          } else {
            ListValue vals(cmd.strs.begin(), cmd.strs.end());
            std::size_t size = vals.size();
            storage_.SetValue(cmd.key, std::move(vals));

            return Reply{UnsignedIntReply{size}};
          }
        }

        for (const auto& val : cmd.strs) {
          reply.value()->emplace_back(val);
        }

        return Reply{UnsignedIntReply{reply.value()->size()}};
      },
      [&](const LPush&) { return Reply{}; },
      [&](const LPop&) { return Reply{}; },
      [&](const RPop&) { return Reply{}; },
      [&](const LLen&) { return Reply{}; },
      [&](const LRange&) { return Reply{}; },
      [&](const LIndex&) { return Reply{}; },
      [&](const LSet&) { return Reply{}; },
      [&](const LInsert&) { return Reply{}; },
      [&](const SAdd&) { return Reply{}; },
      [&](const SRem&) { return Reply{}; },
      [&](const SIsMember&) { return Reply{}; },
      [&](const SMembers&) { return Reply{}; },
      [&](const SCard&) { return Reply{}; },
      [&](const SUnion&) { return Reply{}; },
      [&](const SInter&) { return Reply{}; },
      [&](const SDiff&) { return Reply{}; },
      [&](const SMove&) { return Reply{}; },
      [&](const GeoAdd&) { return Reply{}; },
      [&](const GeoPos&) { return Reply{}; },
      [&](const GeoDist&) { return Reply{}; },
      [&](const GeoSearch&) { return Reply{}; },
      [&](const GeoSearchStore&) { return Reply{}; },
      [&](const ExpireCommand& cmd) {
        if (!storage_.Exists(cmd.key)) {
          return Reply{IntegerReply{0}};
        }
        storage_.SetKeyExpiration(cmd.key, cmd.seconds);
        return Reply{IntegerReply{1}};
      },
      [&](const TTLCommand& cmd) {
        return Reply{IntegerReply{storage_.GetKeyTTL(cmd.key)}};
      },
      [&](const TypeCommand&) { return Reply{}; },
      [&](const DelCommand&) { return Reply{}; },
      [&](const ExistsCommand&) { return Reply{}; },
      [&](const KeysCommand&) { return Reply{}; },
      [&](const FlushDbCommand&) { return Reply{}; },
      [&](const ConfigSetCommand&) { return Reply{}; },
      [&](const ConfigGetCommand&) { return Reply{}; },
      [&](const DbSizeCommand&) { return Reply{}; },
      [&](const MemoryUsageCommand&) { return Reply{}; }};

  return std::visit(overload, command);
}
