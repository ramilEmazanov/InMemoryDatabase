#include "command_executor.hpp"

#include <variant>

#include "overload.hpp"
#include "reply.hpp"
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
      [](const StrlenCommand& strlen_command) { return Reply{}; },
      [](const AppendCommand& append_command) { return Reply{}; },
      [](const ExpireCommand& expire_command) { return Reply{}; },
      [](const TTLCommand& ttl_command) { return Reply{}; }};

  return std::visit(overload, command);
}
