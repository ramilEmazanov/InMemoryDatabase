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

class SetTest : public ::testing::Test {
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

TEST_F(SetTest, SAddCreatesNewSet) {
  EXPECT_EQ(Run("SADD myset a"), "(integer) 1\n");
  EXPECT_EQ(Run("SISMEMBER myset a"), "(integer) 1\n");
}

TEST_F(SetTest, SAddMultipleNewElements) {
  EXPECT_EQ(Run("SADD myset a b c"), "(integer) 3\n");
  EXPECT_EQ(Run("SCARD myset"), "(integer) 3\n");
}

TEST_F(SetTest, SAddOnlyNew) {
  Run("SADD myset a b");
  EXPECT_EQ(Run("SADD myset b c"), "(integer) 1\n");
  EXPECT_EQ(Run("SCARD myset"), "(integer) 3\n");
}

TEST_F(SetTest, SAddAllDuplicates) {
  Run("SADD myset a b c");
  EXPECT_EQ(Run("SADD myset a b c"), "(integer) 0\n");
}

TEST_F(SetTest, SRemMissingKey) {
  EXPECT_EQ(Run("SREM emptykey a"), "(integer) 0\n");
}

TEST_F(SetTest, SRemExistingCount) {
  Run("SADD myset a b c");
  EXPECT_EQ(Run("SREM myset a b"), "(integer) 2\n");
  EXPECT_EQ(Run("SCARD myset"), "(integer) 1\n");
}

TEST_F(SetTest, SRemNonExisting) {
  Run("SADD myset a");
  EXPECT_EQ(Run("SREM myset x"), "(integer) 0\n");
}

TEST_F(SetTest, SIsMemberAbsentMember) {
  Run("SADD myset a");
  EXPECT_EQ(Run("SISMEMBER myset b"), "(integer) 0\n");
}

TEST_F(SetTest, SMembersMissingKey) {
  EXPECT_EQ(Run("SMEMBERS emptykey"), "(empty array)\n");
}

TEST_F(SetTest, SCardMissingKey) {
  EXPECT_EQ(Run("SCARD emptykey"), "(integer) 0\n");
}

TEST_F(SetTest, SCardReturnsSize) {
  Run("SADD myset a b c d");
  EXPECT_EQ(Run("SCARD myset"), "(integer) 4\n");
}

TEST_F(SetTest, SCardRemove) {
  Run("SADD myset a b c");
  Run("SREM myset a");
  EXPECT_EQ(Run("SCARD myset"), "(integer) 2\n");
}

TEST_F(SetTest, SUnionSingleMissing) {
  EXPECT_EQ(Run("SUNION emptykey"), "(empty array)\n");
}

TEST_F(SetTest, SUnionSingleKey) {
  Run("SADD myset a");
  EXPECT_EQ(Run("SUNION myset"), "1) \"a\"\n");
}

TEST_F(SetTest, SUnionMultipleKeysReturnsUnion) {
  Run("SADD s1 a b");
  Run("SADD s2 b c");
  std::string result = Run("SUNION s1 s2");
  EXPECT_THAT(result, HasSubstr("\"a\""));
  EXPECT_THAT(result, HasSubstr("\"b\""));
  EXPECT_THAT(result, HasSubstr("\"c\""));
  EXPECT_NE(result.find("1) "), std::string::npos);
  EXPECT_NE(result.find("2) "), std::string::npos);
  EXPECT_NE(result.find("3) "), std::string::npos);
  EXPECT_EQ(result.find("4) "), std::string::npos);
}

TEST_F(SetTest, SUnionAllMissingKeysReturnsEmpty) {
  EXPECT_EQ(Run("SUNION nokey1 nokey2"), "(empty array)\n");
}

TEST_F(SetTest, SInterSingleKey) {
  Run("SADD myset a");
  EXPECT_EQ(Run("SINTER myset"), "1) \"a\"\n");
}

TEST_F(SetTest, SInterMissingKey) {
  Run("SADD s1 a b");
  EXPECT_EQ(Run("SINTER s1 emptykey"), "(empty array)\n");
}

TEST_F(SetTest, SInterNoCommon) {
  Run("SADD s1 a b");
  Run("SADD s2 c d");
  EXPECT_EQ(Run("SINTER s1 s2"), "(empty array)\n");
}

TEST_F(SetTest, SInterBase) {
  Run("SADD s1 a b");
  Run("SADD s2 b c");
  EXPECT_EQ(Run("SINTER s1 s2"), "1) \"b\"\n");
}

TEST_F(SetTest, SDiffSingleKeyReturns) {
  Run("SADD myset a");
  EXPECT_EQ(Run("SDIFF myset"), "1) \"a\"\n");
}

TEST_F(SetTest, SDiffFirstKeyMissing) {
  EXPECT_EQ(Run("SDIFF emptykey"), "(empty array)\n");
}

TEST_F(SetTest, SDiffReturnsUnique) {
  Run("SADD s1 a b c");
  Run("SADD s2 b c d");
  EXPECT_EQ(Run("SDIFF s1 s2"), "1) \"a\"\n");
}

TEST_F(SetTest, SDiffWrongType) {
  Run("SET mykey val");
  EXPECT_THAT(Run("SDIFF mykey"), HasSubstr("WRONGTYPE"));
}

TEST_F(SetTest, SMoveMissingSourceo) {
  EXPECT_EQ(Run("SMOVE emptykey dst a"), "(integer) 0\n");
}

TEST_F(SetTest, SMoveSameSrcAndDest) {
  Run("SADD myset a");
  EXPECT_EQ(Run("SMOVE myset myset a"), "(integer) 1\n");
  EXPECT_EQ(Run("SISMEMBER myset a"), "(integer) 1\n");
  EXPECT_EQ(Run("SCARD myset"), "(integer) 1\n");
}

TEST_F(SetTest, SMoveMemberNoDupl) {
  Run("SADD src a b");
  Run("SADD dst a");
  EXPECT_EQ(Run("SMOVE src dst a"), "(integer) 1\n");
  EXPECT_EQ(Run("SCARD dst"), "(integer) 1\n");
  EXPECT_EQ(Run("SISMEMBER src a"), "(integer) 0\n");
}
