#pragma once

#include <optional>
#include <string>
#include <vector>

struct GeoAdd {
  struct Point {
    double longitude;
    double latitude;
    std::string member;
  };

  std::string key;
  std::vector<Point> points;
};

struct GeoPos {
  std::string key;
  std::vector<std::string> members;
};

struct GeoDist {
  std::string key;
  std::string member1;
  std::string member2;
  std::string unit;
};

struct GeoSearch {
  std::string key;
  double longitude;
  double latitude;
  double radius;
  std::string unit;
  std::optional<bool> sort_asc;
  std::optional<std::size_t> limit;
};

struct GeoSearchStore {
  std::string dest;
  std::string source;
  double longitude;
  double latitude;
  double radius;
  std::string unit;
  std::optional<bool> sort_asc;
  std::optional<std::size_t> limit;
};
