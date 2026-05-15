#pragma once

#include <string_commands.hpp>
#include <variant>

using ParsedCommand = std::variant<SetCommand, GetCommand, StrlenCommand,
                                   AppendCommand, ExpireCommand>;
