#include <parsed_command.hpp>

struct Reply {};

class CommandExecutor {
  public:
  Reply Execute(const ParsedCommand& command);

  private:

};