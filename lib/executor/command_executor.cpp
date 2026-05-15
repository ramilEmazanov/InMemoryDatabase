#include "command_executor.hpp"
#include "overload.hpp"
#include "reply.hpp"
#include <variant>

Reply CommandExecutor::Execute(const ParsedCommand& command) {
  auto overload = Overload{
    [](const SetCommand& set_command) {},
    [](const GetCommand& get_command) {},
    [](const StrlenCommand& strlen_command) {},
    [](const AppendCommand& append_command) {},
    [](const ExpireCommand& expire_command) {},
    [](const TTLCommand& ttl_command) {}
  };

  std::visit(overload, command);
}
