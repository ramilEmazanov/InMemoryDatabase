#include "parsed_command.hpp"
#include "reply.hpp"

class CommandExecutor {
 public:
  Reply Execute(const ParsedCommand& command);

 private:
};