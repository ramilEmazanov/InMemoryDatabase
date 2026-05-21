#include "set_executor.hpp"

#include <set>
#include <unordered_map>
#include <unordered_set>

#include "command_executor.hpp"
#include "errors.hpp"
#include "reply.hpp"
#include "types.hpp"

SetExecutor::SetExecutor(CommandExecutor& executor) : executor_(executor) {}

Reply SetExecutor::operator()(const SAdd& cmd) {
  auto result = executor_.storage_.GetValueAs<SetValue>(cmd.key);

  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      SetValue set;
      for (const auto& val : cmd.members) set.insert(val);
      std::size_t inserted = set.size();
      auto put = executor_.storage_.PutValue(cmd.key, std::move(set));
      if (!put.has_value()) {
        return Reply{ErrorReply{FormatStorageError(put.error())}};
      }
      return Reply{UnsignedIntReply{inserted}};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }

  SetValue* set = result.value();
  std::vector<std::string> absence;
  absence.reserve(cmd.members.size());
  for (const auto& val : cmd.members) {
    if (!set->contains(val)) absence.push_back(val);
  }

  auto added = executor_.storage_.MutateValueAdd<SetValue>(
      cmd.key, absence,
      [&](SetValue& s) {
        for (const auto& val : absence) s.emplace(val);
      });
  if (!added.has_value()) {
    return Reply{ErrorReply{FormatStorageError(added.error())}};
  }
  return Reply{UnsignedIntReply{absence.size()}};
}

Reply SetExecutor::operator()(const SRem& cmd) {
  auto result = executor_.storage_.GetValueAs<SetValue>(cmd.key);
  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      return Reply{UnsignedIntReply{0}};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }

  SetValue* set = result.value();
  std::vector<std::string> to_remove;
  to_remove.reserve(cmd.members.size());
  for (const auto& val : cmd.members) {
    if (set->contains(val)) to_remove.push_back(val);
  }

  executor_.storage_.MutateValueSubtr<SetValue>(
      cmd.key, to_remove,
      [&](SetValue& s) {
        for (const auto& val : to_remove) s.erase(val);
      });
  return Reply{UnsignedIntReply{to_remove.size()}};
}

Reply SetExecutor::operator()(const SIsMember& cmd) {
  auto result = executor_.storage_.GetValueAs<SetValue>(cmd.key);
  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      return Reply{UnsignedIntReply{0}};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }
  return Reply{UnsignedIntReply{result.value()->contains(cmd.member) ? 1u : 0u}};
}

Reply SetExecutor::operator()(const SMembers& cmd) {
  auto result = executor_.storage_.GetValueAs<SetValue>(cmd.key);
  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      return Reply{ArrayReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }

  std::vector<std::string> res;
  res.reserve(result.value()->size());
  for (const auto& val : *result.value()) res.push_back(val);
  return Reply{ArrayReply{std::move(res)}};
}

Reply SetExecutor::operator()(const SCard& cmd) {
  auto result = executor_.storage_.GetValueAs<SetValue>(cmd.key);
  if (!result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(result.error())) {
      return Reply{UnsignedIntReply{0}};
    }
    return Reply{ErrorReply{FormatStorageError(result.error())}};
  }
  return Reply{UnsignedIntReply{result.value()->size()}};
}

Reply SetExecutor::operator()(const SUnion& cmd) {
  if (cmd.keys.empty()) return Reply{ArrayReply{}};

  if (cmd.keys.size() == 1) {
    auto result = executor_.storage_.GetValueAs<SetValue>(cmd.keys[0]);
    if (!result.has_value()) {
      if (std::holds_alternative<NoSuchKey>(result.error())) {
        return Reply{ArrayReply{}};
      }
      return Reply{ErrorReply{FormatStorageError(result.error())}};
    }
    std::vector<std::string> res;
    for (const auto& val : *result.value()) res.push_back(val);
    return Reply{ArrayReply{std::move(res)}};
  }

  std::set<std::string> res;
  for (const auto& key : cmd.keys) {
    auto result = executor_.storage_.GetValueAs<SetValue>(key);
    if (result.has_value()) {
      for (const auto& item : *result.value()) res.insert(item);
    }
  }

  std::vector<std::string> ans(res.begin(), res.end());
  return Reply{ArrayReply{std::move(ans)}};
}

Reply SetExecutor::operator()(const SInter& cmd) {
  if (cmd.keys.empty()) return Reply{ArrayReply{}};

  if (cmd.keys.size() == 1) {
    auto result = executor_.storage_.GetValueAs<SetValue>(cmd.keys[0]);
    if (!result.has_value()) {
      if (std::holds_alternative<NoSuchKey>(result.error())) {
        return Reply{ArrayReply{}};
      }
      return Reply{ErrorReply{FormatStorageError(result.error())}};
    }
    std::vector<std::string> res;
    for (const auto& val : *result.value()) res.push_back(val);
    return Reply{ArrayReply{std::move(res)}};
  }

  for (const auto& key : cmd.keys) {
    auto resp = executor_.storage_.GetValueAs<SetValue>(key);
    if (!resp.has_value() && std::holds_alternative<NoSuchKey>(resp.error())) {
      return Reply{ArrayReply{}};
    }
  }

  std::unordered_map<std::string, std::size_t> counter;
  for (const auto& key : cmd.keys) {
    auto result = executor_.storage_.GetValueAs<SetValue>(key);
    for (const auto& item : *result.value()) counter[item]++;
  }

  std::vector<std::string> res;
  for (const auto& [item, cnt] : counter) {
    if (cnt == cmd.keys.size()) res.push_back(item);
  }
  return Reply{ArrayReply{std::move(res)}};
}

Reply SetExecutor::operator()(const SDiff& cmd) {
  if (cmd.keys.empty()) return Reply{ArrayReply{}};

  if (cmd.keys.size() == 1) {
    auto result = executor_.storage_.GetValueAs<SetValue>(cmd.keys[0]);
    if (!result.has_value()) {
      if (std::holds_alternative<NoSuchKey>(result.error())) {
        return Reply{ArrayReply{}};
      }
      return Reply{ErrorReply{FormatStorageError(result.error())}};
    }
    std::vector<std::string> res;
    for (const auto& val : *result.value()) res.push_back(val);
    return Reply{ArrayReply{std::move(res)}};
  }

  auto main_result = executor_.storage_.GetValueAs<SetValue>(cmd.keys[0]);
  if (!main_result.has_value()) {
    if (std::holds_alternative<NoSuchKey>(main_result.error())) {
      return Reply{ArrayReply{}};
    }
    return Reply{ErrorReply{FormatStorageError(main_result.error())}};
  }

  std::unordered_set<std::string> temp(main_result.value()->begin(),
                                       main_result.value()->end());

  for (std::size_t i = 1; i < cmd.keys.size(); ++i) {
    auto result = executor_.storage_.GetValueAs<SetValue>(cmd.keys[i]);
    if (!result.has_value()) continue;
    for (const auto& item : *result.value()) temp.erase(item);
  }

  std::vector<std::string> res(temp.begin(), temp.end());
  return Reply{ArrayReply{std::move(res)}};
}

Reply SetExecutor::operator()(const SMove& cmd) {
  auto source_resp = executor_.storage_.GetValueAs<SetValue>(cmd.source);
  if (!source_resp.has_value()) {
    if (std::holds_alternative<NoSuchKey>(source_resp.error())) {
      return Reply{UnsignedIntReply{0}};
    }
    return Reply{ErrorReply{FormatStorageError(source_resp.error())}};
  }

  SetValue* source = source_resp.value();
  if (!source->contains(cmd.member)) return Reply{UnsignedIntReply{0}};

  auto dest_resp = executor_.storage_.GetValueAs<SetValue>(cmd.destination);
  if (!dest_resp.has_value()) {
    if (std::holds_alternative<NoSuchKey>(dest_resp.error())) {
      std::unordered_set<std::string> new_set{cmd.member};
      executor_.storage_.PutValue(cmd.destination, std::move(new_set));
      executor_.storage_.MutateValueSubtr<SetValue>(
          cmd.source, {cmd.member},
          [&](SetValue& s) { s.erase(cmd.member); });
      if (source->empty()) executor_.storage_.EraseValue(cmd.source);
      return Reply{UnsignedIntReply{1}};
    }
    return Reply{ErrorReply{FormatStorageError(dest_resp.error())}};
  }

  SetValue* dest = dest_resp.value();
  if (source == dest) return Reply{UnsignedIntReply{1}};

  if (!dest->contains(cmd.member)) {
    executor_.storage_.MutateValueAdd<SetValue>(
        cmd.destination, {cmd.member},
        [&](SetValue& s) { s.insert(cmd.member); });
  }
  executor_.storage_.MutateValueSubtr<SetValue>(
      cmd.source, {cmd.member},
      [&](SetValue& s) { s.erase(cmd.member); });

  if (source->empty()) executor_.storage_.EraseValue(cmd.source);
  return Reply{UnsignedIntReply{1}};
}
