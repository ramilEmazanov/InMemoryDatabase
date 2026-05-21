#include "geo_executor.hpp"

#include <algorithm>

#include "command_executor.hpp"
#include "errors.hpp"
#include "geo_methods.hpp"
#include "reply.hpp"
#include "types.hpp"

GeoExecutor::GeoExecutor(CommandExecutor& executor) : executor_(executor) {}

Reply GeoExecutor::operator()(const GeoAdd& cmd) {
  std::vector<std::pair<std::string, GeoPoint>> points;
  points.reserve(cmd.points.size());
  for (const auto& p : cmd.points) {
    points.emplace_back(p.member, GeoPoint{p.latitude, p.longitude});
  }

  auto result = executor_.storage_.GeoAddPoints(cmd.key, points);
  if (!result.has_value()) {
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }
  return Reply{UnsignedIntReply{result.value()}};
}

Reply GeoExecutor::operator()(const GeoPos& cmd) {
  auto result = executor_.storage_.GetValueAs<GeoValue>(cmd.key);

  GeoPosReply reply;
  reply.entries.reserve(cmd.members.size());

  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      for (std::size_t i = 0; i < cmd.members.size(); ++i) {
        reply.entries.push_back(std::nullopt);
      }
      return Reply{std::move(reply)};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }

  const GeoValue& geo = *result.value();
  for (const auto& member : cmd.members) {
    auto it = geo.find(member);
    if (it != geo.end()) {
      reply.entries.push_back(std::make_pair(it->second.lon, it->second.lat));
    } else {
      reply.entries.push_back(std::nullopt);
    }
  }
  return Reply{GeoPosReply{std::move(reply)}};
}

Reply GeoExecutor::operator()(const GeoDist& cmd) {
  auto result = executor_.storage_.GetValueAs<GeoValue>(cmd.key);
  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      return Reply{NilReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }

  const GeoValue& geo = *result.value();
  auto it1 = geo.find(cmd.member1);
  auto it2 = geo.find(cmd.member2);
  if (it1 == geo.end() || it2 == geo.end()) return Reply{NilReply{}};

  double dist_km = GeoSpatial::HaversineDistanceKm(
      it1->second.lon, it1->second.lat, it2->second.lon, it2->second.lat);
  return Reply{BulkStringReply{std::to_string(GeoSpatial::ConvertFromKm(dist_km, cmd.unit))}};
}

Reply GeoExecutor::operator()(const GeoSearch& cmd) {
  auto result = executor_.storage_.GetValueAs<GeoValue>(cmd.key);
  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      return Reply{ArrayReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }

  const GeoValue& geo = *result.value();
  double radius_km = GeoSpatial::ConvertToKm(cmd.radius, cmd.unit);

  std::vector<std::pair<double, std::string>> candidates;
  for (const auto& [member, point] : geo) {
    double dist_km = GeoSpatial::HaversineDistanceKm(
        cmd.longitude, cmd.latitude, point.lon, point.lat);
    if (dist_km <= radius_km) candidates.emplace_back(dist_km, member);
  }

  if (cmd.sort_asc.has_value()) {
    if (*cmd.sort_asc) {
      std::sort(candidates.begin(), candidates.end());
    } else {
      std::sort(candidates.begin(), candidates.end(),
                std::greater<std::pair<double, std::string>>{});
    }
  }

  if (cmd.limit.has_value() && candidates.size() > *cmd.limit) {
    candidates.resize(*cmd.limit);
  }

  std::vector<std::string> members;
  members.reserve(candidates.size());
  for (const auto& [dist, member] : candidates) members.push_back(member);
  return Reply{ArrayReply{std::move(members)}};
}

Reply GeoExecutor::operator()(const GeoSearchStore& cmd) {
  auto result = executor_.storage_.GetValueAs<GeoValue>(cmd.source);
  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      executor_.storage_.EraseValue(cmd.dest);
      return Reply{UnsignedIntReply{0}};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }

  const GeoValue& geo = *result.value();
  double radius_km = GeoSpatial::ConvertToKm(cmd.radius, cmd.unit);

  std::vector<std::pair<double, std::string>> candidates;
  for (const auto& [member, point] : geo) {
    double dist_km = GeoSpatial::HaversineDistanceKm(
        cmd.longitude, cmd.latitude, point.lon, point.lat);
    if (dist_km <= radius_km) candidates.emplace_back(dist_km, member);
  }

  if (cmd.sort_asc.has_value()) {
    if (*cmd.sort_asc) {
      std::sort(candidates.begin(), candidates.end());
    } else {
      std::sort(candidates.begin(), candidates.end(),
                std::greater<std::pair<double, std::string>>{});
    }
  }

  if (cmd.limit.has_value() && candidates.size() > *cmd.limit) {
    candidates.resize(*cmd.limit);
  }

  if (candidates.empty()) {
    executor_.storage_.EraseValue(cmd.dest);
    return Reply{UnsignedIntReply{0}};
  }

  GeoValue new_geo;
  for (const auto& [dist, member] : candidates) new_geo[member] = geo.at(member);
  std::size_t count = new_geo.size();

  auto store_result = executor_.storage_.PutValue(cmd.dest, std::move(new_geo));
  if (!store_result.has_value()) {
    return Reply{ErrorReply{FormatStorageError(store_result.error())}};
  }
  return Reply{UnsignedIntReply{count}};
}
