#include "string_cmd.hpp"

Result SetCmdStr(Storage& s, Args& args) {}

Result GetCmdStr(Storage& s, Args& args) {}

Result StrlenStr(Storage& s, Args& args) {}

Result AppendStr(Storage& s, Args& args) {}

Result ExpireStr(Storage& s, Args& args) {}

Result TtlStr(Storage& s, Args& args) {}

void AddStringCommands(Commands& cmd) {
  cmd.Add({"SET", 1, 1, &SetCmdStr});
}
