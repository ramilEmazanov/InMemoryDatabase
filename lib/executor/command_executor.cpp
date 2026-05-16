#include "command_executor.hpp"

#include <variant>

#include "overload.hpp"
#include "reply.hpp"
#include "storage.hpp"

Reply CommandExecutor::Execute(const ParsedCommand& command) {
  auto overload =
      Overload{[&](const SetCommand& set_command) { 
        std::string key = set_command.key;
        std::string val = set_command.value;

        storage_.SetValue(key, StringValue{val});
        
        return Reply{VoidReply{}}; 
      },
               [](const GetCommand& get_command) { return Reply{}; },
               [](const StrlenCommand& strlen_command) { return Reply{}; },
               [](const AppendCommand& append_command) { return Reply{}; },
               [](const ExpireCommand& expire_command) { return Reply{}; },
               [](const TTLCommand& ttl_command) { return Reply{}; }};

  return std::visit(overload, command);
}
