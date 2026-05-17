#pragma once

#include <variant>

#include "general_commands.hpp"
#include "geo_commands.hpp"
#include "list_commands.hpp"
#include "set_commands.hpp"
#include "string_commands.hpp"

using ParsedCommand =
    std::variant<SetCommand, GetCommand, StrlenCommand, AppendCommand,
                 ExpireCommand, TTLCommand, LPush, RPush, LPop, RPop, LLen,
                 LRange, LIndex, LSet, LInsert, SAdd, SRem, SIsMember, SMembers,
                 SCard, SUnion, SInter, SDiff, SMove, GeoAdd, GeoPos, GeoDist,
                 GeoSearch, GeoSearchStore, TypeCommand, DelCommand,
                 ExistsCommand, KeysCommand, FlushDbCommand, ConfigSetCommand,
                 ConfigGetCommand, DbSizeCommand, MemoryUsageCommand>;
