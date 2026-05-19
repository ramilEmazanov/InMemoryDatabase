#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct ExpireCommand {
  std::string key;
  std::size_t seconds;
};

struct TTLCommand {
  std::string key;
};

struct TypeCommand {
  std::string key;
};

struct DelCommand {
  std::vector<std::string> keys;
};

struct ExistsCommand {
  std::vector<std::string> keys;
};

struct KeysCommand {
  std::string pattern;
};

struct FlushDbCommand {};

struct ConfigSetCommand {
  std::string value;
};

struct ConfigGetCommand {
  std::string parameter;
};

struct DbSizeCommand {};

struct MemoryUsageCommand {
  std::string key;
};