#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <thread>

#include "command_executor.hpp"
#include "memory_controller.hpp"
#include "runner.hpp"
#include "storage.hpp"
#include "time_controller.hpp"

using ::testing::HasSubstr;
using ::testing::Not;

class TimeCommandTest : public ::testing::Test {
 protected:
  void SetUp() override {
    memctr = std::make_unique<MemoryController>(0);
    timectr = std::make_unique<TimeController>();
    storage = std::make_unique<Storage>(*memctr, *timectr);
    executor = std::make_unique<CommandExecutor>(*storage, *memctr);
  }

  std::string Run(const std::string& cmd) {
    std::istringstream in(cmd);
    std::ostringstream out, err;
    Runner runner(in, out, err, *executor);
    runner.Run();
    return out.str();
  }

  std::unique_ptr<MemoryController> memctr;
  std::unique_ptr<TimeController> timectr;
  std::unique_ptr<Storage> storage;
  std::unique_ptr<CommandExecutor> executor;
};

TEST_F(TimeCommandTest, TtlMissingKey) {
  EXPECT_EQ(Run("TTL nosuchkey"), "(integer) -2\n");
}

TEST_F(TimeCommandTest, TtlKeyWithoutExpiration) {
  Run("SET k v");
  EXPECT_EQ(Run("TTL k"), "(integer) -1\n");
}

TEST_F(TimeCommandTest, TtlAfterExpire) {
  Run("SET k v");
  Run("EXPIRE k 100");
  EXPECT_THAT(Run("TTL k"), Not(HasSubstr("-")));
  EXPECT_THAT(Run("TTL k"), HasSubstr("(integer)"));
}

TEST_F(TimeCommandTest, ExpiredKeyRemoved) {
  Run("SET k v");
  Run("EXPIRE k 1");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 1\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  Run("EXISTS k");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 0\n");
}

TEST_F(TimeCommandTest, OverwriteExpireExtend) {
  Run("SET k v");
  Run("EXPIRE k 1");
  Run("EXPIRE k 100");
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  EXPECT_EQ(Run("GET k"), "\"v\"\n");
}