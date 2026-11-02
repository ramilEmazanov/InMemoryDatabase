#pragma once

#include <cmath>
#include <string>

class GeoSpatial {
 public:
  static double ConvertToKm(double value, const std::string& unit);

  static double ConvertFromKm(double km, const std::string& unit);

  static double HaversineDistanceKm(double lon1, double lat1, double lon2,
                                    double lat2);

  static double ToRadians(double deg);
};