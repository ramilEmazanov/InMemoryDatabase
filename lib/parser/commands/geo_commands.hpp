#pragma once

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
  bool asc;
  std::size_t count;
};

struct GeoSearchStore {
  std::string dest;
  std::string source;
  double longitude;
  double latitude;
  double radius;
  std::string unit;
  bool asc;
  std::size_t count;
};
