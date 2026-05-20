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

class ListTest : public ::testing::Test {
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

TEST_F(ListTest, LPushCreatesList) {
  EXPECT_EQ(Run("LPUSH mylist a"), "(integer) 1\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"a\"\n");
}

TEST_F(ListTest, LPushMultipleValues) {
  Run("LPUSH mylist a b c");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"c\"\n2) \"b\"\n3) \"a\"\n");
}

TEST_F(ListTest, RPushCreatesList) {
  EXPECT_EQ(Run("RPUSH mylist a"), "(integer) 1\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"a\"\n");
}

TEST_F(ListTest, RPushMultipleValuesKeepsOrder) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"a\"\n2) \"b\"\n3) \"c\"\n");
}

TEST_F(ListTest, LPushAndRPush) {
  Run("RPUSH mylist a b");
  Run("LPUSH mylist c");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"c\"\n2) \"a\"\n3) \"b\"\n");
}

TEST_F(ListTest, LLenExisting) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LLEN mylist"), "(integer) 3\n");
}

TEST_F(ListTest, LLenMissing) {
  EXPECT_EQ(Run("LLEN nosuchkey"), "(integer) 0\n");
}

TEST_F(ListTest, LPopOne) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LPOP mylist"), "a\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"b\"\n2) \"c\"\n");
}

TEST_F(ListTest, LPopCount) {
  Run("RPUSH mylist a b c d");
  EXPECT_EQ(Run("LPOP mylist 2"), "1) \"a\"\n2) \"b\"\n");
  EXPECT_EQ(Run("LLEN mylist"), "(integer) 2\n");
}

TEST_F(ListTest, LPopCountGreaterThanSize) {
  Run("RPUSH mylist a b");
  EXPECT_EQ(Run("LPOP mylist 10"), "1) \"a\"\n2) \"b\"\n");
}

TEST_F(ListTest, LPopMissingReturnsNil) {
  EXPECT_EQ(Run("LPOP nosuchkey"), "(nil)\n");
}

TEST_F(ListTest, LPopLastElementDeletesKey) {
  Run("RPUSH mylist only");
  EXPECT_EQ(Run("LPOP mylist"), "only\n");
  EXPECT_EQ(Run("LLEN mylist"), "(integer) 0\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "(empty array)\n");
}

TEST_F(ListTest, RPopOne) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("RPOP mylist"), "c\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"a\"\n2) \"b\"\n");
}

TEST_F(ListTest, RPopCount) {
  Run("RPUSH mylist a b c d");
  EXPECT_EQ(Run("RPOP mylist 2"), "1) \"c\"\n2) \"d\"\n");
  EXPECT_EQ(Run("LLEN mylist"), "(integer) 2\n");
}

TEST_F(ListTest, RPopCountGreaterThanSize) {
  Run("RPUSH mylist a b");
  EXPECT_EQ(Run("RPOP mylist 10"), "1) \"a\"\n2) \"b\"\n");
}

TEST_F(ListTest, RPopMissingReturnsNil) {
  EXPECT_EQ(Run("RPOP nosuchkey"), "(nil)\n");
}

TEST_F(ListTest, RPopLastElementDeletesKey) {
  Run("RPUSH mylist only");
  EXPECT_EQ(Run("RPOP mylist"), "only\n");
  EXPECT_EQ(Run("LLEN mylist"), "(integer) 0\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "(empty array)\n");
}

TEST_F(ListTest, LRangePositiveIndexes) {
  Run("RPUSH mylist a b c d e");
  EXPECT_EQ(Run("LRANGE mylist 1 3"), "1) \"b\"\n2) \"c\"\n3) \"d\"\n");
}

TEST_F(ListTest, LRangeNegativeIndexes) {
  Run("RPUSH mylist a b c d e");
  EXPECT_EQ(Run("LRANGE mylist -3 -1"), "1) \"c\"\n2) \"d\"\n3) \"e\"\n");
}

TEST_F(ListTest, LRangeClampsOutOfBounds) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LRANGE mylist -100 100"), "1) \"a\"\n2) \"b\"\n3) \"c\"\n");
}

TEST_F(ListTest, LRangeEmptyWhenStartAfterStop) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LRANGE mylist 3 1"), "(empty array)\n");
}

TEST_F(ListTest, LRangeMissingReturnsEmpty) {
  EXPECT_EQ(Run("LRANGE nosuchkey 0 -1"), "(empty array)\n");
}

TEST_F(ListTest, LIndexPositive) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LINDEX mylist 1"), "b\n");
}

TEST_F(ListTest, LIndexNegative) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LINDEX mylist -1"), "c\n");
}

TEST_F(ListTest, LIndexOutOfRangeReturnsNil) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LINDEX mylist 10"), "(nil)\n");
}

TEST_F(ListTest, LIndexMissingReturnsNil) {
  EXPECT_EQ(Run("LINDEX nosuchkey 0"), "(nil)\n");
}

TEST_F(ListTest, LSetPositiveIndex) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LSET mylist 1 B"), "OK\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"a\"\n2) \"B\"\n3) \"c\"\n");
}

TEST_F(ListTest, LSetNegativeIndex) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LSET mylist -1 C"), "OK\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "1) \"a\"\n2) \"b\"\n3) \"C\"\n");
}

TEST_F(ListTest, LSetOutOfRangeReturnsError) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LSET mylist 10 x"), "(error) ERR index out of range\n");
}

TEST_F(ListTest, LSetMissingReturnsError) {
  EXPECT_EQ(Run("LSET nosuchkey 0 x"), "(error) ERR no such key\n");
}

TEST_F(ListTest, LInsertBefore) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LINSERT mylist BEFORE b X"), "(integer) 4\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"),
            "1) \"a\"\n2) \"X\"\n3) \"b\"\n4) \"c\"\n");
}

TEST_F(ListTest, LInsertAfter) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LINSERT mylist AFTER b X"), "(integer) 4\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"),
            "1) \"a\"\n2) \"b\"\n3) \"X\"\n4) \"c\"\n");
}

TEST_F(ListTest, LInsertUsesFirstPivot) {
  Run("RPUSH mylist a b a c");
  EXPECT_EQ(Run("LINSERT mylist BEFORE a X"), "(integer) 5\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"),
            "1) \"X\"\n2) \"a\"\n3) \"b\"\n4) \"a\"\n5) \"c\"\n");
}

TEST_F(ListTest, LInsertMissingPivotReturnsMinusOne) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("LINSERT mylist BEFORE z X"), "(integer) -1\n");
}

TEST_F(ListTest, LInsertMissingKeyReturnsZero) {
  EXPECT_EQ(Run("LINSERT nosuchkey BEFORE a X"), "(integer) 0\n");
}

TEST_F(ListTest, LInsertInvalidPositionReturnsError) {
  EXPECT_THAT(RunErr("LINSERT mylist CENTER a X"), HasSubstr("Bad keyword"));
}

TEST_F(ListTest, WrongTypeErrors) {
  Run("SET mykey val");
  EXPECT_THAT(Run("LLEN mykey"), HasSubstr("WRONGTYPE"));
  EXPECT_THAT(Run("LPUSH mykey x"), HasSubstr("WRONGTYPE"));
  EXPECT_THAT(Run("LRANGE mykey 0 -1"), HasSubstr("WRONGTYPE"));
}

TEST_F(ListTest, WrongArgumentCount) {
  EXPECT_THAT(RunErr("RPUSH mylist"), HasSubstr("Wrong arity"));
  EXPECT_THAT(RunErr("LLEN"), HasSubstr("Wrong arity"));
  EXPECT_THAT(RunErr("LPOP"), HasSubstr("Wrong arity"));
  EXPECT_THAT(RunErr("LRANGE mylist 0"), HasSubstr("Wrong arity"));
  EXPECT_THAT(RunErr("LINDEX mylist"), HasSubstr("Wrong arity"));
  EXPECT_THAT(RunErr("LSET mylist 0"), HasSubstr("Wrong arity"));
  EXPECT_THAT(RunErr("LINSERT mylist BEFORE x"), HasSubstr("Wrong arity"));
}

TEST_F(ListTest, InvalidIntegerArguments) {
  EXPECT_THAT(RunErr("LRANGE mylist abc 1"), HasSubstr("Bad integer"));
  EXPECT_THAT(RunErr("LRANGE mylist 0 abc"), HasSubstr("Bad integer"));
  EXPECT_THAT(RunErr("LINDEX mylist abc"), HasSubstr("Bad integer"));
  EXPECT_THAT(RunErr("LSET mylist abc val"), HasSubstr("Bad integer"));
  EXPECT_THAT(RunErr("LPOP mylist abc"), HasSubstr("Bad integer"));
  EXPECT_THAT(RunErr("RPOP mylist abc"), HasSubstr("Bad integer"));
}

TEST_F(ListTest, TypeReturnsList) {
  Run("RPUSH mylist a");
  EXPECT_EQ(Run("TYPE mylist"), "list\n");
}

TEST_F(ListTest, DelRemovesList) {
  Run("RPUSH mylist a b c");
  EXPECT_EQ(Run("DEL mylist"), "(integer) 1\n");
  EXPECT_EQ(Run("LLEN mylist"), "(integer) 0\n");
}

TEST_F(ListTest, ExistsSeesList) {
  Run("RPUSH mylist a");
  EXPECT_EQ(Run("EXISTS mylist"), "(integer) 1\n");
}

TEST_F(ListTest, DbSizeCountsList) {
  Run("RPUSH mylist a");
  EXPECT_EQ(Run("DBSIZE"), "(integer) 1\n");
}

TEST_F(ListTest, ExpiredListBehavesAsMissing) {
  Run("RPUSH mylist a b c");
  Run("EXPIRE mylist 1");
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  EXPECT_EQ(Run("LLEN mylist"), "(integer) 0\n");
  EXPECT_EQ(Run("LRANGE mylist 0 -1"), "(empty array)\n");
}

TEST_F(ListTest, MutatingListKeepsTtl) {
  Run("RPUSH mylist a");
  Run("EXPIRE mylist 100");
  Run("RPUSH mylist b");
  std::string ttl = Run("TTL mylist");
  EXPECT_NE(ttl, "(integer) -1\n");
  EXPECT_NE(ttl, "(integer) -2\n");
}

TEST_F(ListTest, RPushRespectsMaxMemory) {
  memctr->SetLimitBytes(1);
  EXPECT_THAT(Run("RPUSH mylist a"), HasSubstr("error"));
}

TEST_F(ListTest, LPushRespectsMaxMemory) {
  memctr->SetLimitBytes(1);
  EXPECT_THAT(Run("LPUSH mylist a"), HasSubstr("error"));
}

TEST_F(ListTest, LSetRespectsMaxMemory) {
  Run("RPUSH mylist a");
  memctr->SetLimitBytes(memctr->GetStorageMemoryUsage());
  EXPECT_THAT(Run("LSET mylist 0 toolongvalue"), HasSubstr("error"));
}

TEST_F(ListTest, FailedPushDoesNotMutateList) {
  Run("RPUSH mylist a b c");
  memctr->SetLimitBytes(memctr->GetStorageMemoryUsage());
  Run("RPUSH mylist d");
  EXPECT_EQ(Run("LLEN mylist"), "(integer) 3\n");
}

TEST_F(ListTest, FailedLSetDoesNotMutateList) {
  Run("RPUSH mylist a");
  memctr->SetLimitBytes(memctr->GetStorageMemoryUsage());
  Run("LSET mylist 0 toolongvalue");
  EXPECT_EQ(Run("LINDEX mylist 0"), "a\n");
}
