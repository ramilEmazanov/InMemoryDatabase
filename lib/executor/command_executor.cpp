#include "command_executor.hpp"

#include <algorithm>
#include <variant>

#include "errors.hpp"
#include "geo_commands.hpp"
#include "geo_methods.hpp"
#include "list_commands.hpp"
#include "overload.hpp"
#include "reply.hpp"
#include "set_commands.hpp"
#include "storage.hpp"

Reply CommandExecutor::Execute(const ParsedCommand& command) {
  auto overload = Overload{
      [&](const SetCommand& cmd) {
        auto reply = storage_.GetValueAs<StringValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            auto adding = storage_.MutateValueAdd<StringValue>(
                cmd.key, {cmd.value},
                [&](StringValue& val) { val.append(cmd.value); });

            if (!adding.has_value()) {
              return Reply{ErrorReply{FormatStorageError(reply.error())}};
            } else {
              return Reply{VoidReply{}};
            }
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::size_t old_bytes = reply.value()->size();
        std::size_t new_bytes = cmd.value.size();

        auto mutate = storage_.MutateValueInPlace<StringValue>(
            cmd.key, old_bytes, new_bytes,
            [&](StringValue& str) { str = cmd.value; });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        return Reply{VoidReply{}};
      },
      [&](const GetCommand& get_command) {
        auto reply = storage_.GetValueAs<std::string>(get_command.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        return Reply{StringReply{*reply.value()}};
      },
      [&](const StrlenCommand& strlen_command) {
        auto reply = storage_.GetValueAs<std::string>(strlen_command.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{UnsignedIntReply{0}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        return Reply{UnsignedIntReply{reply.value()->size()}};
      },
      [&](const AppendCommand& cmd) {
        auto reply = storage_.GetValueAs<std::string>(cmd.key);

        if (!reply.has_value() &&
            std::holds_alternative<NoSuchKey>(reply.error())) {
          auto adding = storage_.MutateValueAdd<StringValue>(
              cmd.key, {cmd.value},
              [&](StringValue& val) { val.append(cmd.value); });

          if (!adding.has_value()) {
            return Reply{ErrorReply{FormatStorageError(reply.error())}};
          } else {
            return Reply{UnsignedIntReply{cmd.value.size()}};
          }
        } else if (!reply.has_value()) {
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::string now = reply.has_value() ? *reply.value() : std::string{};
        std::string added = now + cmd.value;

        auto set_reply = storage_.MutateValueInPlace<StringValue>(
            cmd.key, now.size(), added.size(),
            [&](StringValue& s) { s = added; });

        if (!set_reply.has_value()) {
          return Reply{ErrorReply{FormatStorageError(set_reply.error())}};
        }

        return Reply{UnsignedIntReply{added.size()}};
      },

      [&](const RPush& cmd) {
        auto result = storage_.MutateValueAdd<ListValue>(
            cmd.key, cmd.strs, [&](ListValue& list) {
              for (const auto& s : cmd.strs) {
                list.push_back(s);
              }
            });
        if (!result.has_value()) {
          return Reply{ErrorReply{FormatStorageError(result.error())}};
        }
        return Reply{UnsignedIntReply{result.value()}};
      },
      [&](const LPush& cmd) {
        auto result = storage_.MutateValueAdd<ListValue>(
            cmd.key, cmd.strs, [&](ListValue& list) {
              for (const auto& s : cmd.strs) {
                list.push_front(s);
              }
            });
        if (!result.has_value()) {
          return Reply{ErrorReply{FormatStorageError(result.error())}};
        }
        return Reply{UnsignedIntReply{result.value()}};
      },
      [&](const LPop& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::vector<std::string> response;

        std::size_t count = cmd.count.value_or(1);

        auto begin = reply.value()->begin();
        auto end = count > reply.value()->size() ? reply.value()->end()
                                                 : begin + count;

        while (begin != end) {
          response.emplace_back(*begin);
          begin++;
        }

        auto mutate = storage_.MutateValueSubtr<ListValue>(
            cmd.key, response, [&](ListValue& list) {
              std::size_t n = std::min(count, list.size());
              for (std::size_t i = 0; i < n; ++i) {
                list.pop_front();
              }
            });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        if (!cmd.count.has_value()) {
          return Reply{BulkStringReply{std::move(response.front())}};
        }

        return Reply{ArrayReply{std::move(response)}};
      },
      [&](const RPop& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::vector<std::string> response;

        std::size_t count = cmd.count.value_or(1);

        auto begin = reply.value()->begin();
        auto end = reply.value()->end();

        begin = count > reply.value()->size()
                    ? begin
                    : begin + (reply.value()->size() - count);

        while (begin != end) {
          response.emplace_back(*begin);
          begin++;
        }

        auto mutate = storage_.MutateValueSubtr<ListValue>(
            cmd.key, std::vector<std::string>{}, [&](ListValue& list) {
              std::size_t n = std::min(count, list.size());
              for (std::size_t i = 0; i < n; ++i) {
                list.pop_back();
              }
            });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        if (!cmd.count.has_value()) {
          return Reply{BulkStringReply{std::move(response.front())}};
        }

        return Reply{ArrayReply{std::move(response)}};
      },
      [&](const LLen& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{UnsignedIntReply{0}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        return Reply{UnsignedIntReply{reply.value()->size()}};
      },
      [&](const LRange& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{ArrayReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        long long size = reply.value()->size();

        long long start = cmd.start < 0 ? cmd.start + size : cmd.start;
        long long stop = cmd.stop < 0 ? cmd.stop + size : cmd.stop;

        start = std::max(0LL, start);
        stop = std::min(stop, size - 1);

        if (start > stop) {
          return Reply{ArrayReply{}};
        }

        std::vector<std::string> result;
        auto it = reply.value()->begin();
        it += start;

        for (long long i = start; i <= stop; ++i, ++it) {
          result.push_back(*it);
        }

        return Reply{ArrayReply{std::move(result)}};
      },
      [&](const LIndex& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::size_t size = reply.value()->size();
        long long ind = cmd.ind < 0 ? cmd.ind + size : cmd.ind;

        if (ind < 0 || ind >= size) {
          return Reply{NilReply{}};
        }

        return Reply{BulkStringReply{reply.value()->at(ind)}};
      },
      [&](const LSet& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{ErrorReply{"ERR no such key"}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        std::size_t size = reply.value()->size();
        long long ind = cmd.ind < 0 ? cmd.ind + size : cmd.ind;

        if (ind < 0 || ind >= size) {
          return Reply{ErrorReply{"ERR index out of range"}};
        }

        std::size_t idx = static_cast<std::size_t>(ind);
        std::size_t old_bytes = reply.value()->at(idx).size();
        std::size_t new_bytes = cmd.val.size();

        auto mutate = storage_.MutateValueInPlace<ListValue>(
            cmd.key, old_bytes, new_bytes,
            [&](ListValue& list) { list[idx] = cmd.val; });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        return Reply{VoidReply{}};
      },
      [&](const LInsert& cmd) {
        auto reply = storage_.GetValueAs<ListValue>(cmd.key);

        if (!reply.has_value()) {
          if (std::holds_alternative<NoSuchKey>(reply.error())) {
            return Reply{IntegerReply{0}};
          }
          return Reply{ErrorReply{FormatStorageError(reply.error())}};
        }

        auto& list = *reply.value();
        auto it = std::find(list.begin(), list.end(), cmd.pivot);

        if (it == list.end()) {
          return Reply{IntegerReply{-1}};
        }

        if (!cmd.before) {
          ++it;
        }

        auto mutate = storage_.MutateValueAdd<ListValue>(
            cmd.key, {cmd.value},
            [&](ListValue& lst) { lst.insert(it, cmd.value); });

        if (!mutate.has_value()) {
          return Reply{ErrorReply{FormatStorageError(mutate.error())}};
        }

        return Reply{IntegerReply{static_cast<long long>(mutate.value())}};
      },
      [&](const SAdd& cmd) {
        auto result = storage_.GetValueAs<SetValue>(cmd.key);

        if (!result.has_value()) {
          if (std::holds_alternative<NoSuchKey>(result.error())) {
            SetValue set;
            for (const auto& val : cmd.members) {
              set.insert(val);
            }
            storage_.PutValue(cmd.key, std::move(set));

            return Reply{UnsignedIntReply{cmd.members.size()}};
          } else {
            return Reply{ErrorReply{FormatStorageError(result.error())}};
          }
        }

        SetValue* set = result.value();
        std::vector<std::string> absence;
        absence.reserve(cmd.members.size());

        for (const auto& val : cmd.members) {
          if (!set->contains(val)) {
            absence.push_back(val);
          }
        }

        auto added = storage_.MutateValueAdd<SetValue>(
            cmd.key, absence, [&](SetValue& set) {
              for (const auto& val : absence) {
                set.emplace(val);
              }
            });
        if (!added.has_value()) {
          return Reply{ErrorReply{FormatStorageError(added.error())}};
        }
        return Reply{UnsignedIntReply{absence.size()}};
      },
      [&](const SRem& cmd) {
        auto result = storage_.GetValueAs<SetValue>(cmd.key);

        if (!result.has_value()) {
          if (std::holds_alternative<NoSuchKey>(result.error())) {
            return Reply{UnsignedIntReply{0}};
          } else {
            return Reply{ErrorReply{FormatStorageError(result.error())}};
          }
        }

        SetValue* set = result.value();
        std::vector<std::string> to_remove;
        to_remove.reserve(cmd.members.size());

        for (const auto& val : cmd.members) {
          if (set->contains(val)) {
            to_remove.push_back(val);
          }
        }

        auto remove = storage_.MutateValueSubtr<SetValue>(
            cmd.key, to_remove, [&](SetValue& set) {
              for (const auto& val : to_remove) {
                set.erase(val);
              }
            });

        return Reply{UnsignedIntReply(to_remove.size())};
      },
      [&](const SIsMember& cmd) {
        auto result = storage_.GetValueAs<SetValue>(cmd.key);

        if (!result.has_value()) {
          if (std::holds_alternative<NoSuchKey>(result.error())) {
            return Reply{UnsignedIntReply{0}};
          } else {
            return Reply{ErrorReply{FormatStorageError(result.error())}};
          }
        }

        SetValue* set = result.value();

        if (set->contains(cmd.member)) {
          return Reply{UnsignedIntReply{1}};
        } else {
          return Reply{UnsignedIntReply{0}};
        }
      },
      [&](const SMembers& cmd) {
        auto result = storage_.GetValueAs<SetValue>(cmd.key);

        if (!result.has_value()) {
          if (std::holds_alternative<NoSuchKey>(result.error())) {
            return Reply{ArrayReply{}};
          } else {
            return Reply{ErrorReply{FormatStorageError(result.error())}};
          }
        }

        SetValue* set = result.value();
        std::vector<std::string> res;
        res.reserve(set->size());

        for (const auto& val : *set) {
          res.push_back(val);
        }

        return Reply{ArrayReply{std::move(res)}};
      },
      [&](const SCard& cmd) {
        auto result = storage_.GetValueAs<SetValue>(cmd.key);

        if (!result.has_value()) {
          if (std::holds_alternative<NoSuchKey>(result.error())) {
            return Reply{UnsignedIntReply{0}};
          } else {
            return Reply{ErrorReply{FormatStorageError(result.error())}};
          }
        }

        SetValue* set = result.value();
        return Reply{UnsignedIntReply{set->size()}};
      },
      [&](const SUnion& cmd) {
        if (cmd.keys.size() == 0) {
          return Reply{ArrayReply{}};
        }
        if (cmd.keys.size() == 1) {
          auto result = storage_.GetValueAs<SetValue>(cmd.keys[0]);

          if (!result.has_value()) {
            if (std::holds_alternative<NoSuchKey>(result.error())) {
              return Reply{ArrayReply{}};
            } else {
              return Reply{ErrorReply{FormatStorageError(result.error())}};
            }
          }

          std::vector<std::string> res;
          SetValue* set = result.value();
          res.reserve(set->size());

          for (const auto& val : *set) {
            res.push_back(val);
          }
          return Reply{ArrayReply{std::move(res)}};
        }

        std::set<std::string> res;

        for (const auto& val : cmd.keys) {
          auto result = storage_.GetValueAs<SetValue>(val);

          if (result.has_value()) {
            auto* set = result.value();

            for (const auto& item : *set) {
              res.insert(item);
            }
          }
        }

        std::vector<std::string> ans;

        ans.reserve(res.size());
        for (const auto& val : res) {
          ans.push_back(val);
        }

        return Reply{ArrayReply{std::move(ans)}};
      },
      [&](const SInter& cmd) {
        std::unordered_map<std::string, std::size_t> counter;

        if (cmd.keys.size() == 0) {
          return Reply{ArrayReply{}};
        }
        if (cmd.keys.size() == 1) {
          auto result = storage_.GetValueAs<SetValue>(cmd.keys[0]);

          if (!result.has_value()) {
            if (std::holds_alternative<NoSuchKey>(result.error())) {
              return Reply{ArrayReply{}};
            } else {
              return Reply{ErrorReply{FormatStorageError(result.error())}};
            }
          }

          std::vector<std::string> res;
          SetValue* set = result.value();
          res.reserve(set->size());

          for (const auto& val : *set) {
            res.push_back(val);
          }
          return Reply{ArrayReply{std::move(res)}};
        }

        for (const auto& key : cmd.keys) {
          auto resp = storage_.GetValueAs<SetValue>(key);
          if (!resp.has_value()) {
            if (std::holds_alternative<NoSuchKey>(resp.error())) {
              return Reply{ArrayReply{}};
            }
          }
        }

        for (const auto& key : cmd.keys) {
          auto result = storage_.GetValueAs<SetValue>(key);

          SetValue* set = result.value();

          for (const auto& item : *set) {
            counter[item]++;
          }
        }

        std::size_t set_count = cmd.keys.size();
        std::vector<std::string> res;

        for (auto it = counter.begin(); it != counter.end(); ++it) {
          if (it->second == set_count) {
            res.push_back(it->first);
          }
        }

        return Reply{ArrayReply{std::move(res)}};
      },
      [&](const SDiff& cmd) {
        if (cmd.keys.size() == 0) {
          return Reply{ArrayReply{}};
        }
        if (cmd.keys.size() == 1) {
          auto result = storage_.GetValueAs<SetValue>(cmd.keys[0]);

          if (!result.has_value()) {
            if (std::holds_alternative<NoSuchKey>(result.error())) {
              return Reply{ArrayReply{}};
            } else {
              return Reply{ErrorReply{FormatStorageError(result.error())}};
            }
          }

          std::vector<std::string> res;
          SetValue* set = result.value();
          res.reserve(set->size());

          for (const auto& val : *set) {
            res.push_back(val);
          }
          return Reply{ArrayReply{std::move(res)}};
        }

        auto main_set = storage_.GetValueAs<SetValue>(cmd.keys[0]);
        std::unordered_set<std::string> temp = *(main_set.value());

        for (std::size_t i = 1; i < cmd.keys.size(); ++i) {
          auto result = storage_.GetValueAs<SetValue>(cmd.keys[i]);

          if (!result.has_value()) {
            continue;
          }

          SetValue* temp_set = result.value();
          for (const auto& item : *temp_set) {
            if (temp.contains(item)) {
              temp.erase(item);
            }
          }
        }

        std::vector<std::string> res;
        res.reserve(temp.size());

        for (const auto& item : temp) {
          res.push_back(item);
        }

        return Reply{ArrayReply{std::move(res)}};
      },
      [&](const SMove& cmd) {
        auto source_resp = storage_.GetValueAs<SetValue>(cmd.source);

        if (!source_resp.has_value()) {
          if (std::holds_alternative<NoSuchKey>(source_resp.error())) {
            return Reply{UnsignedIntReply{0}};
          } else {
            return Reply{ErrorReply{FormatStorageError(source_resp.error())}};
          }
        }

        SetValue* source = source_resp.value();
        if (!source->contains(cmd.member)) {
          return Reply{UnsignedIntReply{0}};
        }

        auto dest_resp = storage_.GetValueAs<SetValue>(cmd.destination);
        if (!dest_resp.has_value()) {
          if (std::holds_alternative<NoSuchKey>(dest_resp.error())) {
            std::unordered_set<std::string> new_;
            new_.insert(cmd.member);
            storage_.PutValue(cmd.destination, std::move(new_));
            return Reply{UnsignedIntReply{1}};
          } else {
            return Reply{ErrorReply{FormatStorageError(source_resp.error())}};
          }
        }

        SetValue* dest = dest_resp.value();

        if (source == dest) {
          if (source->contains(cmd.member)) {
            return Reply{UnsignedIntReply{1}};
          } else {
            return Reply{UnsignedIntReply{0}};
          }
        }

        bool deleted = false;

        if (dest->contains(cmd.member)) {
          storage_.MutateValueSubtr<SetValue>(
              cmd.source, {cmd.member},
              [&](SetValue& set) { set.erase(cmd.member); });
          deleted = true;
        }

        storage_.MutateValueAdd<SetValue>(
            cmd.destination, {cmd.member},
            [&](SetValue& set) { set.insert(cmd.member); });

        if (!deleted) {
          storage_.MutateValueSubtr<SetValue>(
              cmd.source, {cmd.member},
              [&](SetValue& set) { set.erase(cmd.member); });
        }

        if (source->empty()) {
          storage_.EraseValue(cmd.source);
        }
        return Reply{UnsignedIntReply{1}};
      },
      [&](const GeoAdd& cmd) {
        std::vector<std::pair<std::string, GeoPoint>> points;

        points.reserve(cmd.points.size());

        for (const auto& p : cmd.points) {
          points.emplace_back(p.member, GeoPoint{p.latitude, p.longitude});
        }
        auto result = storage_.GeoAddPoints(cmd.key, points);

        if (!result.has_value()) {
          return Reply{ErrorReply{FormatStorageError(result.error())}};
        }

        return Reply{UnsignedIntReply{result.value()}};
      },
      [&](const GeoPos& cmd) {
        auto result = storage_.GetValueAs<GeoValue>(cmd.key);

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
            reply.entries.push_back(
                std::make_pair(it->second.lon, it->second.lat));
          } else {
            reply.entries.push_back(std::nullopt);
          }
        }

        return Reply{GeoPosReply{std::move(reply)}};
      },
      [&](const GeoDist& cmd) {
        auto result = storage_.GetValueAs<GeoValue>(cmd.key);

        if (!result.has_value()) {
          if (std::holds_alternative<NoSuchKey>(result.error())) {
            return Reply{NilReply{}};
          }
          return Reply{ErrorReply{FormatStorageError(result.error())}};
        }

        const GeoValue& geo = *result.value();
        auto it1 = geo.find(cmd.member1);
        auto it2 = geo.find(cmd.member2);

        if (it1 == geo.end() || it2 == geo.end()) {
          return Reply{NilReply{}};
        }

        double dist_km = GeoSpatial::HaversineDistanceKm(
            it1->second.lon, it1->second.lat, it2->second.lon, it2->second.lat);
        double dist = GeoSpatial::ConvertFromKm(dist_km, cmd.unit);

        return Reply{BulkStringReply{std::to_string(dist)}};
      },
      [&](const GeoSearch& cmd) {
        auto result = storage_.GetValueAs<GeoValue>(cmd.key);

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
          if (dist_km <= radius_km) {
            candidates.emplace_back(dist_km, member);
          }
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
        for (const auto& [dist, member] : candidates) {
          members.push_back(member);
        }

        return Reply{ArrayReply{std::move(members)}};
      },
      [&](const GeoSearchStore& cmd) {
        auto result = storage_.GetValueAs<GeoValue>(cmd.source);

        if (!result.has_value()) {
          if (std::holds_alternative<NoSuchKey>(result.error())) {
            storage_.EraseValue(cmd.dest);
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
          if (dist_km <= radius_km) {
            candidates.emplace_back(dist_km, member);
          }
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
          storage_.EraseValue(cmd.dest);
          return Reply{UnsignedIntReply{0}};
        }

        GeoValue new_geo;
        for (const auto& [dist, member] : candidates) {
          new_geo[member] = geo.at(member);
        }
        std::size_t count = new_geo.size();

        auto store_result = storage_.PutValue(cmd.dest, std::move(new_geo));
        if (!store_result.has_value()) {
          return Reply{ErrorReply{FormatStorageError(store_result.error())}};
        }

        return Reply{UnsignedIntReply{count}};
      },
      [&](const ExpireCommand& cmd) {
        if (!storage_.Exists(cmd.key)) {
          return Reply{IntegerReply{0}};
        }
        storage_.SetKeyExpiration(cmd.key, cmd.seconds);
        return Reply{IntegerReply{1}};
      },
      [&](const TTLCommand& cmd) {
        return Reply{IntegerReply{storage_.GetKeyTTL(cmd.key)}};
      },
      [&](const TypeCommand& cmd) {
        return Reply{BulkStringReply{storage_.GetTypeName(cmd.key)}};
      },
      [&](const DelCommand& cmd) {
        std::size_t count = 0;
        for (const auto& key : cmd.keys) {
          count += storage_.EraseValue(key) ? 1 : 0;
        }
        return Reply{UnsignedIntReply{count}};
      },
      [&](const ExistsCommand& cmd) {
        std::size_t count = 0;
        for (const auto& key : cmd.keys) {
          count += storage_.Exists(key) ? 1 : 0;
        }
        return Reply{UnsignedIntReply{count}};
      },
      [&](const KeysCommand& cmd) {
        return Reply{ArrayReply{storage_.FindKeysMatching(cmd.pattern)}};
      },
      [&](const FlushDbCommand&) {
        storage_.Flush();
        return Reply{VoidReply{}};
      },
      [&](const ConfigSetCommand& cmd) {
        storage_.SetLimitMemory(cmd.value);
        return Reply{VoidReply{}};
      },
      [&](const ConfigGetCommand&) {
        return Reply{UnsignedIntReply{storage_.GetMaxMemory()}};
      },
      [&](const DbSizeCommand&) {
        return Reply{UnsignedIntReply{storage_.GetTotalKeyCount()}};
      },
      [&](const MemoryUsageCommand& cmd) {
        return Reply{UnsignedIntReply{storage_.CountKeyMemoryUsage(cmd.key)}};
      }};

  return std::visit(overload, command);
}
