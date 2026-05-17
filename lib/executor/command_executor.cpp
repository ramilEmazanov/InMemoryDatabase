#include "command_executor.hpp"

#include <variant>

#include "geo_commands.hpp"
#include "list_commands.hpp"
#include "overload.hpp"
#include "reply.hpp"
#include "set_commands.hpp"
#include "storage.hpp"

Reply CommandExecutor::Execute(const ParsedCommand& command) {
  auto overload = Overload{
      [&](const SetCommand& set_command) {
        storage_.SetValue(set_command.key, StringValue{set_command.value});
        return Reply{VoidReply{}};
      },
      [&](const GetCommand& get_command) {
        auto* reply = storage_.GetValueAs<std::string>(get_command.key);

        return Reply{StringReply{*reply}};
      },
      [](const StrlenCommand&) { return Reply{}; },
      [](const AppendCommand&) { return Reply{}; },
      [](const ExpireCommand&) { return Reply{}; },
      [](const TTLCommand&) { return Reply{}; },
      [](const LPush&) { return Reply{}; },
      [](const RPush&) { return Reply{}; },
      [](const LPop&) { return Reply{}; },
      [](const RPop&) { return Reply{}; },
      [](const LLen&) { return Reply{}; },
      [](const LRange&) { return Reply{}; },
      [](const LIndex&) { return Reply{}; },
      [](const LSet&) { return Reply{}; },
      [](const LInsert&) { return Reply{}; },
      [](const SAdd&) { return Reply{}; },
      [](const SRem&) { return Reply{}; },
      [](const SIsMember&) { return Reply{}; },
      [](const SMembers&) { return Reply{}; },
      [](const SCard&) { return Reply{}; },
      [](const SUnion&) { return Reply{}; },
      [](const SInter&) { return Reply{}; },
      [](const SDiff&) { return Reply{}; },
      [](const SMove&) { return Reply{}; },
      [](const GeoAdd&) { return Reply{}; },
      [](const GeoPos&) { return Reply{}; },
      [](const GeoDist&) { return Reply{}; },
      [](const GeoSearch&) { return Reply{}; },
      [](const GeoSearchStore&) { return Reply{}; },
      [](const TypeCommand&) { return Reply{}; },
      [](const DelCommand&) { return Reply{}; },
      [](const ExistsCommand&) { return Reply{}; },
      [](const KeysCommand&) { return Reply{}; },
      [](const FlushDbCommand&) { return Reply{}; },
      [](const ConfigSetCommand&) { return Reply{}; },
      [](const ConfigGetCommand&) { return Reply{}; },
      [](const DbSizeCommand&) { return Reply{}; },
      [](const MemoryUsageCommand&) { return Reply{}; }};

  return std::visit(overload, command);
}
