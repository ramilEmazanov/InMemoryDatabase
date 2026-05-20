#include "command_executor.hpp"

#include <algorithm>
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
        auto result = storage_.MutateValueAdd<ListValue>(
            cmd.key, cmd.strs, [&](ListValue& list) {
              for (const auto& s : cmd.strs) {
                list.push_back(s);
              }
            });
        if (!result.has_value()) {
          return Reply{ErrorReply{FormatStorageError(result.error())}};
        }
        return Reply{UnsignedIntReply{result.value()}};
      },
      [&](const LPush& cmd) {
        auto result = storage_.MutateValueAdd<ListValue>(
            cmd.key, cmd.strs, [&](ListValue& list) {
              for (const auto& s : cmd.strs) {
                list.push_front(s);
              }
            });
        if (!result.has_value()) {
          return Reply{ErrorReply{FormatStorageError(result.error())}};
        }
        return Reply{UnsignedIntReply{result.value()}};
      },
      [&](const LPop& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::vector<std::string> response;

        std::size_t count = cmd.count.value_or(1);

        auto begin = reply.value()->begin();
        auto end = count > reply.value()->size() ? reply.value()->end()
                                                 : begin + count;

        while (begin != end) {
          response.emplace_back(*begin);
          begin++;
        }

        auto mutate = storage_.MutateValueSubtr<ListValue>(
            cmd.key, std::vector<std::string>{}, [&](ListValue& list) {
              std::size_t n = std::min(count, list.size());
              for (std::size_t i = 0; i < n; ++i) {
                list.pop_front();
              }
            });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        if (!cmd.count.has_value()) {
          return Reply{BulkStringReply{std::move(response.front())}};
        }

        return Reply{ArrayReply{std::move(response)}};
      },
      [&](const RPop& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::vector<std::string> response;

        std::size_t count = cmd.count.value_or(1);

        auto begin = reply.value()->begin();
        auto end = reply.value()->end();

        begin = count > reply.value()->size()
                    ? begin
                    : begin + (reply.value()->size() - count);

        while (begin != end) {
          response.emplace_back(*begin);
          begin++;
        }

        auto mutate = storage_.MutateValueSubtr<ListValue>(
            cmd.key, std::vector<std::string>{}, [&](ListValue& list) {
              std::size_t n = std::min(count, list.size());
              for (std::size_t i = 0; i < n; ++i) {
                list.pop_back();
              }
            });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        if (!cmd.count.has_value()) {
          return Reply{BulkStringReply{std::move(response.front())}};
        }

        return Reply{ArrayReply{std::move(response)}};
      },
      [&](const LLen& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{UnsignedIntReply{0}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        return Reply{UnsignedIntReply{reply.value()->size()}};
      },
      [&](const LRange& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

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

        if (start > stop) {
          return Reply{ArrayReply{}};
        }

        std::vector<std::string> result;
        auto it = reply.value()->begin();
        it += start;

        for (long long i = start; i <= stop; ++i, ++it) {
          result.push_back(*it);
        }

        return Reply{ArrayReply{std::move(result)}};
      },
      [&](const LIndex& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::size_t size = reply.value()->size();
        std::size_t ind = cmd.ind < 0 ? cmd.ind + size : cmd.ind;

        if (ind < 0 || ind >= size) {
          return Reply{NilReply{}};
        }

        return Reply{BulkStringReply{reply.value()->at(ind)}};
      },
      [&](const LSet& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{ErrorReply{"ERR no such key"}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::size_t size = reply.value()->size();
        long long ind = cmd.ind < 0 ? cmd.ind + size : cmd.ind;

        if (ind < 0 || ind >= size) {
          return Reply{ErrorReply{"ERR index out of range"}};
        }

        std::size_t idx = static_cast<std::size_t>(ind);
        std::size_t old_bytes = reply.value()->at(idx).size();
        std::size_t new_bytes = cmd.val.size();

        auto mutate = storage_.MutateValueInPlace<ListValue>(
            cmd.key, old_bytes, new_bytes,
            [&](ListValue& list) { list[idx] = cmd.val; });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        return Reply{VoidReply{}};
      },
      [&](const LInsert& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{IntegerReply{0}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        auto& list = *reply.value();
        auto it = std::find(list.begin(), list.end(), cmd.pivot);

        if (it == list.end()) {
          return Reply{IntegerReply{-1}};
        }

        if (!cmd.before) {
          ++it;
        }

        auto mutate = storage_.MutateValueAdd<ListValue>(
            cmd.key, {cmd.value},
            [&](ListValue& lst) { lst.insert(it, cmd.value); });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        return Reply{IntegerReply{static_cast<long long>(mutate.value())}};
      },
      [&](const SAdd&) {
        // SetValue
        // Number of elements that were added
        return Reply{};
      },
      [&](const SRem&) {
        // Number of elements that removed not including non existing
        return Reply{};
      },
      [&](const SIsMember&) {
        // 0 if not
        // 1 if yes
        return Reply{};
      },
      [&](const SMembers&) {
        // Elements, otherwise empty array
        return Reply{};
      },
      [&](const SCard&) {
        // if does not exist
        // else cardinality
        return Reply{};
      },
      [&](const SUnion&) {
        // Just a union
        return Reply{};
      },
      [&](const SInter&) {
        // Just an intersection
        return Reply{};
      },
      [&](const SDiff&) {
        // Just a diff
        return Reply{};
      },
      [&](const SMove&) {
        // 1 if moved
        // 0 else
        return Reply{};
      },
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
      [&](const TypeCommand& cmd) {
        // string, list, zzset, hash
        // none if not exist key

        storage_.GetTypeName(cmd.key);
        return Reply{};
      },
      [&](const DelCommand& cmd) {
        for (const auto& key : cmd.keys) {
          storage_.EraseValue(key);
        }
        return Reply{};
      },
      [&](const ExistsCommand& cmd) {
        for (const auto& key : cmd.keys) {
          storage_.Exists(key);
        }
        return Reply{};
      },
      [&](const KeysCommand& cmd) {
        storage_.FindKeysMatching(cmd.pattern);
        return Reply{};
      },
      [&](const FlushDbCommand&) {
        storage_.Flush();
        return Reply{};
      },
      [&](const ConfigSetCommand& cmd) {
        storage_.SetLimitMemory(0);
        return Reply{};
      },
      [&](const ConfigGetCommand&) {
        storage_.GetMaxMemory();
        return Reply{};
      },
      [&](const DbSizeCommand&) {
        storage_.GetTotalKeyCount();
        return Reply{};
      },
      [&](const MemoryUsageCommand& cmd) {
        storage_.CountKeyMemoryUsage(cmd.key);
        return Reply{};
      }};

  return std::visit(overload, command);
}
