#pragma once

#include <string>

struct SetCommand {
  std::string key;
  std::string value;
};

struct GetCommand {
  std::string key;
};

struct StrlenCommand {
  std::string key;
};

struct AppendCommand {
  std::string key;
  std::string value;
};

