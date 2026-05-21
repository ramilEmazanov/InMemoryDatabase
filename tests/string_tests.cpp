#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <sstream>
#include <thread>

#include "command_executor.hpp"
#include "memory_controller.hpp"
#include "runner.hpp"
#include "storage.hpp"
#include "time_controller.hpp"

using ::testing::HasSubstr;

class StringTest : public ::testing::Test {
 protected:
  void SetUp() override {
    memctr = std::make_unique<MemoryController>(5000);
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

  std::string RunErr(const std::string& cmd) {
    std::istringstream in(cmd);
    std::ostringstream out, err;
    Runner runner(in, out, err, *executor);
    runner.Run();
    return err.str();
  }

  std::unique_ptr<MemoryController> memctr;
  std::unique_ptr<TimeController> timectr;
  std::unique_ptr<Storage> storage;
  std::unique_ptr<CommandExecutor> executor;
};

TEST_F(StringTest, SetCreatesValue) {
  Run("SET foo bar");
  EXPECT_EQ(Run("GET foo"), "\"bar\"\n");
}

TEST_F(StringTest, SetOverwritesValue) {
  Run("SET foo bara");
  Run("SET foo bere");
  EXPECT_EQ(Run("GET foo"), "\"bere\"\n");
}

TEST_F(StringTest, GetMissingReturnsNil) {
  EXPECT_EQ(Run("GET nosuchkey"), "(nil)\n");
}

TEST_F(StringTest, KeysAreCaseSensitive) {
  Run("SET Foo bar");
  EXPECT_EQ(Run("GET foo"), "(nil)\n");
}

TEST_F(StringTest, CommandsAreCaseInsensitive) {
  Run("set foo bar");
  EXPECT_EQ(Run("get foo"), "\"bar\"\n");
}

TEST_F(StringTest, StrlenExisting) {
  Run("SET key hello");
  EXPECT_EQ(Run("STRLEN key"), "(integer) 5\n");
}

TEST_F(StringTest, StrlenMissing) {
  EXPECT_EQ(Run("STRLEN nosuchkey"), "(integer) 0\n");
}

TEST_F(StringTest, AppendExisting) {
  Run("SET key Hello");
  Run("APPEND key World");
  EXPECT_EQ(Run("GET key"), "\"HelloWorld\"\n");
}

TEST_F(StringTest, AppendMissingCreatesString) {
  Run("APPEND newkey Hello");
  EXPECT_EQ(Run("GET newkey"), "\"Hello\"\n");
}

TEST_F(StringTest, AppendReturnsNewLength) {
  EXPECT_EQ(Run("APPEND k hi"), "(integer) 2\n");
}

TEST_F(StringTest, ExpireExistingKey) {
  Run("SET key val");
  EXPECT_EQ(Run("EXPIRE key 100"), "(integer) 1\n");
}

TEST_F(StringTest, KeyDisappearsAfterExpiration) {
  Run("SET key val");
  Run("EXPIRE key 1");
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  EXPECT_EQ(Run("GET key"), "(nil)\n");
}

TEST_F(StringTest, SetClearsExistingTtl) {
  Run("SET key val");
  Run("EXPIRE key 1");
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  Run("SET key newval");
  EXPECT_EQ(Run("TTL key"), "(integer) -1\n");
}

TEST_F(StringTest, TypeReturnsString) {
  Run("SET key val");
  EXPECT_EQ(Run("TYPE key"), "string\n");
}

TEST_F(StringTest, DelRemovesString) {
  Run("SET key val");
  EXPECT_EQ(Run("DEL key"), "(integer) 1\n");
  EXPECT_EQ(Run("GET key"), "(nil)\n");
}

TEST_F(StringTest, ExistsSeesString) {
  Run("SET key val");
  EXPECT_EQ(Run("EXISTS key"), "(integer) 1\n");
}

TEST_F(StringTest, DbSizeCountsString) {
  Run("SET key val");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 1\n");
}