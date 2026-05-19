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
              for (std::size_t i = 0; i < count; ++i) {
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

        auto begin = reply.value()->rend();
        auto end = count > reply.value()->size() ? reply.value()->end()
                                                 : begin + count;

        while (begin != end) {
          response.emplace_back(*begin);
          begin++;
        }

        auto mutate = storage_.MutateValueSubtr<ListValue>(
            cmd.key, std::vector<std::string>{}, [&](ListValue& list) {
              for (std::size_t i = 0; i < count; ++i) {
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
      [&](const LLen&) {
        // GetValue + size method
        // Size, otherwise if there is no then 0
        return Reply{};
      },
      [&](const LRange&) {
        // GetValue + проход
        // Включительно правый край
        // If stop is bigger than size -> turn it into the last
        // Array of elements, otherwise empty array
        return Reply{};
      },
      [&](const LIndex&) {
        // GetValue + operator[]
        // NilReply - index out of range
        // BulkString - string
        // Также поддерживаем отрицательные
        return Reply{};
      },
      [&](const LSet&) {
        // Error if out of range
        // в MutateValue передать лямбду которая в Value по индексу изменит
        // и вектор из 1-го элемента
        return Reply{};
      },
      [&](const LInsert&) {
        // Аналогично LSet но в лямбде попросим lower_bound
        // List length if success
        // 0 if does not exist
        // - 1 if pivot not found
        return Reply{};
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
