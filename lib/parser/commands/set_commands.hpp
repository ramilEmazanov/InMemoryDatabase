#pragma once

#include <string>
#include <vector>

struct SAdd {
  std::string key;
  std::vector<std::string> members;
};

struct SRem {
  std::string key;
  std::vector<std::string> members;
};

struct SIsMember {
  std::string key;
  std::string member;
};

struct SMembers {
  std::string key;
};

struct SCard {
  std::string key;
};

struct SUnion {
  std::vector<std::string> keys;
};

struct SInter {
  std::vector<std::string> keys;
};

struct SDiff {
  std::vector<std::string> keys;
};

struct SMove {
  std::string source;
  std::string destination;
  std::string member;
};
