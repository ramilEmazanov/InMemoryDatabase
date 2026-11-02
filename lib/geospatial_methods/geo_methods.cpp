#include "geo_methods.hpp"

#include <cmath>
#include <numbers>
#include <string>

constexpr double kEarthRadiusKm = 6372.8;

double GeoSpatial::ToRadians(double deg) {
  return deg * std::numbers::pi / 180.0;
}

double GeoSpatial::HaversineDistanceKm(double lon1, double lat1, double lon2,
                                       double lat2) {
  double dlat = ToRadians(lat2 - lat1);
  double dlon = ToRadians(lon2 - lon1);
  double rlat1 = ToRadians(lat1);
  double rlat2 = ToRadians(lat2);
  double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
             std::cos(rlat1) * std::cos(rlat2) * std::sin(dlon / 2) *
                 std::sin(dlon / 2);
  return kEarthRadiusKm * 2.0 * std::asin(std::sqrt(a));
}

double GeoSpatial::ConvertFromKm(double km, const std::string& unit) {
  if (unit == "m") return km * 1000.0;
  if (unit == "mi") return km / 1.609344;
  if (unit == "ft") return km * 1000.0 / 0.3048;
  return km;
}

double GeoSpatial::ConvertToKm(double value, const std::string& unit) {
  if (unit == "m") return value / 1000.0;
  if (unit == "mi") return value * 1.609344;
  if (unit == "ft") return value * 0.3048 / 1000.0;
  return value;
}
