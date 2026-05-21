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

class GeoTest : public ::testing::Test {
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

  std::string RunErr(const std::string& cmd) {
    std::istringstream in(cmd);
    std::ostringstream out, err;
    Runner runner(in, out, err, *executor);
    runner.Run();
    return err.str();
  }

  void AddAllCities() {
    Run("GEOADD cities 37.6173 55.7558 moscow");
    Run("GEOADD cities 30.3351 59.9343 spb");
    Run("GEOADD cities 55.9721 54.7388 ufa");
    Run("GEOADD cities 2.3522 48.8566 paris");
  }

  std::unique_ptr<MemoryController> memctr;
  std::unique_ptr<TimeController> timectr;
  std::unique_ptr<Storage> storage;
  std::unique_ptr<CommandExecutor> executor;
};

TEST_F(GeoTest, GeoAdd) {
  EXPECT_EQ(Run("GEOADD cities 37.6173 55.7558 moscow 30.3351 59.9343 spb"),
            "(integer) 2\n");
}

TEST_F(GeoTest, GeoAddDuplicate) {
  Run("GEOADD cities 37.6173 55.7558 moscow");
  EXPECT_EQ(Run("GEOADD cities 37.6173 55.7558 moscow"), "(integer) 0\n");
}

TEST_F(GeoTest, GeoPos) {
  Run("GEOADD cities 37.6173 55.7558 moscow");
  std::string result = Run("GEOPOS cities moscow");
  EXPECT_THAT(result, HasSubstr("37.6173"));
  EXPECT_THAT(result, HasSubstr("55.7558"));
}

TEST_F(GeoTest, GeoPosNonExi) {
  Run("GEOADD cities 37.6173 55.7558 moscow");
  EXPECT_THAT(Run("GEOPOS cities nosuchmember"), HasSubstr("(nil)"));
}

TEST_F(GeoTest, GeoDistMoscowToSpbKm) {
  AddAllCities();
  std::string result = Run("GEODIST cities moscow spb km");
  EXPECT_THAT(result, HasSubstr("633."));
}

TEST_F(GeoTest, GeoDistNonExist) {
  AddAllCities();
  EXPECT_EQ(Run("GEODIST cities moscow nosuchmember km"), "(nil)\n");
}

TEST_F(GeoTest, GeoSearch) {
  AddAllCities();
  std::string result =
      Run("GEOSEARCH cities FROMLONLAT 37.6173 55.7558 BYRADIUS 900 km ASC");
  EXPECT_THAT(result, HasSubstr("\"moscow\""));
  EXPECT_THAT(result, HasSubstr("\"spb\""));
  EXPECT_THAT(result, Not(HasSubstr("\"ufa\"")));
  EXPECT_THAT(result, Not(HasSubstr("\"paris\"")));
}

TEST_F(GeoTest, GeoSearchAscOrder) {
  AddAllCities();
  std::string result =
      Run("GEOSEARCH cities FROMLONLAT 37.6173 55.7558 BYRADIUS 2000 km ASC");
  std::size_t pos_msk = result.find("\"moscow\"");
  std::size_t pos_spb = result.find("\"spb\"");
  std::size_t pos_ufa = result.find("\"ufa\"");
  EXPECT_NE(pos_msk, std::string::npos);
  EXPECT_NE(pos_spb, std::string::npos);
  EXPECT_NE(pos_ufa, std::string::npos);
  EXPECT_LT(pos_msk, pos_spb);
  EXPECT_LT(pos_spb, pos_ufa);
}

TEST_F(GeoTest, GeoSearchStoreBase) {
  AddAllCities();
  std::string result =
      Run("GEOSEARCHSTORE result cities FROMLONLAT 37.6173 55.7558 BYRADIUS "
          "700 km");
  EXPECT_EQ(result, "(integer) 2\n");
}

TEST_F(GeoTest, GeoSearchStoreCreatesnew) {
  AddAllCities();
  Run("GEOSEARCHSTORE nearby cities FROMLONLAT 37.6173 55.7558 BYRADIUS 700 "
      "km");
  std::string pos_result = Run("GEOPOS nearby moscow");
  EXPECT_THAT(pos_result, HasSubstr("37.6173"));
  std::string nil_result = Run("GEOPOS nearby ufa");
  EXPECT_THAT(nil_result, HasSubstr("(nil)"));
}
