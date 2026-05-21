#include "general_executor.hpp"

#include "command_executor.hpp"
#include "errors.hpp"
#include "reply.hpp"

GeneralExecutor::GeneralExecutor(CommandExecutor& executor)
    : executor_(executor) {}

Reply GeneralExecutor::operator()(const ExpireCommand& cmd) {
  if (!executor_.storage_.Exists(cmd.key)) return Reply{IntegerReply{0}};
  executor_.storage_.SetKeyExpiration(cmd.key, cmd.seconds);
  return Reply{IntegerReply{1}};
}

Reply GeneralExecutor::operator()(const TTLCommand& cmd) {
  return Reply{IntegerReply{executor_.storage_.GetKeyTTL(cmd.key)}};
}

Reply GeneralExecutor::operator()(const TypeCommand& cmd) {
  return Reply{BulkStringReply{executor_.storage_.GetTypeName(cmd.key)}};
}

Reply GeneralExecutor::operator()(const DelCommand& cmd) {
  std::size_t count = 0;
  for (const auto& key : cmd.keys) {
    count += executor_.storage_.EraseValue(key) ? 1 : 0;
  }
  return Reply{UnsignedIntReply{count}};
}

Reply GeneralExecutor::operator()(const ExistsCommand& cmd) {
  std::size_t count = 0;
  for (const auto& key : cmd.keys) {
    count += executor_.storage_.Exists(key) ? 1 : 0;
  }
  return Reply{UnsignedIntReply{count}};
}

Reply GeneralExecutor::operator()(const KeysCommand& cmd) {
  return Reply{ArrayReply{executor_.storage_.FindKeysMatching(cmd.pattern)}};
}

Reply GeneralExecutor::operator()(const FlushDbCommand&) {
  executor_.storage_.Flush();
  return Reply{VoidReply{}};
}

Reply GeneralExecutor::operator()(const ConfigSetCommand& cmd) {
  executor_.storage_.SetLimitMemory(cmd.value);
  return Reply{VoidReply{}};
}

Reply GeneralExecutor::operator()(const ConfigGetCommand&) {
  return Reply{UnsignedIntReply{executor_.storage_.GetMaxMemory()}};
}

Reply GeneralExecutor::operator()(const DbSizeCommand&) {
  return Reply{UnsignedIntReply{executor_.storage_.GetTotalKeyCount()}};
}

Reply GeneralExecutor::operator()(const MemoryUsageCommand& cmd) {
  return Reply{UnsignedIntReply{executor_.storage_.CountKeyMemoryUsage(cmd.key)}};
}
