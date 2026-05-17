#include "parser_commands_table.hpp"

#include "general_parser.hpp"
#include "geo_parser.hpp"
#include "list_parser.hpp"
#include "set_parser.hpp"
#include "string_parser.hpp"

const std::unordered_map<std::string, ParseFunction> GetCommandTable() {
  std::unordered_map<std::string, ParseFunction> table;

  table.emplace("SET", &SetCommandParse);
  table.emplace("GET", &GetCommandParse);
  table.emplace("STRLEN", &StrlenCommandParse);
  table.emplace("APPEND", &AppendCommandParse);
  table.emplace("EXPIRE", &ExpireCommandParse);
  table.emplace("TTL", &TTLCommandParse);

  table.emplace("LPUSH", &LPushCommandParse);
  table.emplace("RPUSH", &RPushCommandParse);
  table.emplace("LPOP", &LPopCommandParse);
  table.emplace("RPOP", &RPopCommandParse);
  table.emplace("LLEN", &LLenCommandParse);
  table.emplace("LRANGE", &LRangeCommandParse);
  table.emplace("LINDEX", &LIndexCommandParse);
  table.emplace("LSET", &LSetCommandParse);
  table.emplace("LINSERT", &LInsertCommandParse);

  table.emplace("SADD", &SAddCommandParse);
  table.emplace("SREM", &SRemCommandParse);
  table.emplace("SISMEMBER", &SIsMemberCommandParse);
  table.emplace("SMEMBERS", &SMembersCommandParse);
  table.emplace("SCARD", &SCardCommandParse);
  table.emplace("SUNION", &SUnionCommandParse);
  table.emplace("SINTER", &SInterCommandParse);
  table.emplace("SDIFF", &SDiffCommandParse);
  table.emplace("SMOVE", &SMoveCommandParse);

  table.emplace("GEOADD", &GeoAddCommandParse);
  table.emplace("GEOPOS", &GeoPosCommandParse);
  table.emplace("GEODIST", &GeoDistCommandParse);
  table.emplace("GEOSEARCH", &GeoSearchCommandParse);
  table.emplace("GEOSEARCHSTORE", &GeoSearchStoreCommandParse);

  table.emplace("TYPE", &TypeCommandParse);
  table.emplace("DEL", &DelCommandParse);
  table.emplace("EXISTS", &ExistsCommandParse);
  table.emplace("KEYS", &KeysCommandParse);
  table.emplace("FLUSHDB", &FlushDbCommandParse);
  table.emplace("CONFIG", &ConfigCommandParse);
  table.emplace("DBSIZE", &DbSizeCommandParse);
  table.emplace("MEMORY", &MemoryCommandParse);

  return table;
}
