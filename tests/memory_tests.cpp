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

class MemoryCommandTest : public ::testing::Test {
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

  std::unique_ptr<MemoryController> memctr_;
  std::unique_ptr<TimeController> timectr_;
  std::unique_ptr<Storage> storage_;
  std::unique_ptr<CommandExecutor> executor_;
};

TEST_F(MemoryCommandTest, ConfigGetDefaultZero) {
  EXPECT_EQ(Run("CONFIG GET maxmemory"), "(integer) 0\n");
}

TEST_F(MemoryCommandTest, ConfigSet) {
  Run("CONFIG SET maxmemory 1024");
  EXPECT_EQ(Run("CONFIG GET maxmemory"), "(integer) 1024\n");
}

TEST_F(MemoryCommandTest, OomError) {
  Run("CONFIG SET maxmemory 1");
  EXPECT_THAT(Run("SET k value"), HasSubstr("OOM"));
}

TEST_F(MemoryCommandTest, MemoryUsageStringKey) {
  Run("SET k hello");
  std::string result = Run("MEMORY USAGE k");
  EXPECT_NE(result, "(integer) 0\n");
}

TEST_F(MemoryCommandTest, ConfigSetZero) {
  Run("CONFIG SET maxmemory 1");
  EXPECT_THAT(Run("SET k val"), HasSubstr("OOM"));
  Run("CONFIG SET maxmemory 0");
  EXPECT_EQ(Run("SET k val"), "OK\n");
}

TEST_F(MemoryCommandTest, MemoryUsageString) {
  Run("SET k hello");
  std::size_t expected = sizeof(StringValue) + std::string("hello").size();
  EXPECT_EQ(Run("MEMORY USAGE k"), std::format("(integer) {}\n", expected));
}

TEST_F(MemoryCommandTest, MemoryUsageList) {
  Run("RPUSH l a bb ccc");
  std::size_t expected = sizeof(ListValue) + (sizeof(std::string) + 1) +
                         (sizeof(std::string) + 2) + (sizeof(std::string) + 3);
  EXPECT_EQ(Run("MEMORY USAGE l"), std::format("(integer) {}\n", expected));
}

TEST_F(MemoryCommandTest, MemoryUsageGrows) {
  Run("RPUSH l x");
  std::size_t before = memctr_->GetStorageMemoryUsage();
  Run("RPUSH l yy");
  std::size_t after = memctr_->GetStorageMemoryUsage();
  EXPECT_EQ(after - before, sizeof(std::string) + std::string("yy").size());
}
