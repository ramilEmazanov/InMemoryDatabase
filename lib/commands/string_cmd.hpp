#pragma once
#include <commands_table.hpp>
#include <storage.hpp>
#include <types.hpp>

Result SetCmdStr(Storage& s, Args& args);

Result GetCmdStr(Storage& s, Args& args);

Result StrlenStr(Storage& s, Args& args);

Result AppendStr(Storage& s, Args& args);

Result ExpireStr(Storage& s, Args& args);

Result TtlStr(Storage& s, Args& args);

void AddStringCommands(CommandsTable& cmd);