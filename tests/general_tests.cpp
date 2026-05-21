#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "command_executor.hpp"
#include "memory_controller.hpp"
#include "runner.hpp"
#include "storage.hpp"
#include "time_controller.hpp"

using ::testing::HasSubstr;
using ::testing::Not;

class GeneralTest : public ::testing::Test {
 protected:
  void SetUp() override {
    memctr_ = std::make_unique<MemoryController>(0);
    timectr_ = std::make_unique<TimeController>();
    storage_ = std::make_unique<Storage>(*memctr_, *timectr_);
    executor_ = std::make_unique<CommandExecutor>(*storage_, *memctr_);
  }

  std::string Run(const std::string& cmd) {
    std::istringstream in(cmd);
    std::ostringstream out, err;
    Runner runner(in, out, err, *executor_);
    runner.Run();
    return out.str();
  }

  std::string RunErr(const std::string& cmd) {
    std::istringstream in(cmd);
    std::ostringstream out, err;
    Runner runner(in, out, err, *executor_);
    runner.Run();
    return err.str();
  }

  std::unique_ptr<MemoryController> memctr_;
  std::unique_ptr<TimeController> timectr_;
  std::unique_ptr<Storage> storage_;
  std::unique_ptr<CommandExecutor> executor_;
};

TEST_F(GeneralTest, TypeMissingKey) {
  EXPECT_EQ(Run("TYPE nosuchkey"), "none\n");
}

TEST_F(GeneralTest, TypeString) {
  Run("SET k v");
  EXPECT_EQ(Run("TYPE k"), "string\n");
}

TEST_F(GeneralTest, TypeList) {
  Run("LPUSH mylist a");
  EXPECT_EQ(Run("TYPE mylist"), "list\n");
}

TEST_F(GeneralTest, TypeSet) {
  Run("SADD myset a");
  EXPECT_EQ(Run("TYPE myset"), "set\n");
}

TEST_F(GeneralTest, DelMissingKey) {
  EXPECT_EQ(Run("DEL nosuchkey"), "(integer) 0\n");
}

TEST_F(GeneralTest, DelExistingKeyReturnsOne) {
  Run("SET k v");
  EXPECT_EQ(Run("DEL k"), "(integer) 1\n");
}

TEST_F(GeneralTest, DelRemovesKey) {
  Run("SET k v");
  Run("DEL k");
  EXPECT_EQ(Run("GET k"), "(nil)\n");
}

TEST_F(GeneralTest, DelMultiple) {
  Run("SET a 1");
  Run("SET b 2");
  EXPECT_EQ(Run("DEL a b"), "(integer) 2\n");
}

TEST_F(GeneralTest, DelDecreasesDbSize) {
  Run("SET a 1");
  Run("SET b 2");
  Run("DEL a");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 1\n");
}

TEST_F(GeneralTest, ExistsMissingKey) {
  EXPECT_EQ(Run("EXISTS nosuchkey"), "(integer) 0\n");
}

TEST_F(GeneralTest, ExistsPresentKey) {
  Run("SET k v");
  EXPECT_EQ(Run("EXISTS k"), "(integer) 1\n");
}

TEST_F(GeneralTest, ExistsMultiple) {
  Run("SET a 1");
  Run("SET b 2");
  EXPECT_EQ(Run("EXISTS a b"), "(integer) 2\n");
}

TEST_F(GeneralTest, DbSizeEmpty) { EXPECT_EQ(Run("DBSIZE"), "(integer) 0\n"); }

TEST_F(GeneralTest, DbSizeAfterSet) {
  Run("SET k v");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 1\n");
}

TEST_F(GeneralTest, DbSizeMultipleTypes) {
  Run("SET s v");
  Run("LPUSH l a");
  Run("SADD st m");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 3\n");
}

TEST_F(GeneralTest, DbSizeAfterDel) {
  Run("SET a 1");
  Run("SET b 2");
  Run("DEL a");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 1\n");
}

TEST_F(GeneralTest, FlushDbClearsAllKeys) {
  Run("SET a 1");
  Run("LPUSH l x");
  Run("SADD s m");
  Run("FLUSHDB");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 0\n");
}

TEST_F(GeneralTest, ConfigSetLimitBlocksWrite) {
  Run("CONFIG SET maxmemory 1");
  EXPECT_THAT(Run("SET k somevalue"), HasSubstr("error"));
}

TEST_F(GeneralTest, MemoryUsageGrows) {
  Run("RPUSH mylist a");
  std::string before = Run("MEMORY USAGE mylist");
  Run("RPUSH mylist b c d e f g");
  std::string after = Run("MEMORY USAGE mylist");
  EXPECT_NE(before, after);
}

TEST_F(GeneralTest, KeysEmptyDb) {
  EXPECT_EQ(Run("KEYS *"), "(empty array)\n");
}

TEST_F(GeneralTest, KeysAfterFlushDb) {
  Run("SET a 1");
  Run("FLUSHDB");
  EXPECT_EQ(Run("KEYS *"), "(empty array)\n");
}

TEST_F(GeneralTest, KeysExactPattern) {
  Run("SET foo 1");
  Run("SET bar 2");
  std::string result = Run("KEYS foo");
  EXPECT_THAT(result, HasSubstr("\"foo\""));
  EXPECT_THAT(result, Not(HasSubstr("\"bar\"")));
}

TEST_F(GeneralTest, KeysWildcardMatchesAll) {
  Run("SET foo 1");
  Run("SET bar 2");
  std::string result = Run("KEYS *");
  EXPECT_THAT(result, HasSubstr("\"foo\""));
  EXPECT_THAT(result, HasSubstr("\"bar\""));
}

TEST_F(GeneralTest, KeysPatternPrefix) {
  Run("SET foo1 1");
  Run("SET foo2 2");
  Run("SET bar 3");
  std::string result = Run("KEYS foo*");
  EXPECT_THAT(result, HasSubstr("\"foo1\""));
  EXPECT_THAT(result, HasSubstr("\"foo2\""));
  EXPECT_THAT(result, Not(HasSubstr("\"bar\"")));
}
