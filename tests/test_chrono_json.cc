#include <radar/mms/common/json/chrono.h>

#include <chrono>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Chrono durations serialize as their count", "[chrono][json]")
{
  auto const duration = std::chrono::seconds(42);

  CHECK(nlohmann::json(duration) == 42);
  CHECK(nlohmann::json(42).get<std::chrono::seconds>() == duration);
  CHECK_THROWS(nlohmann::json("42").get<std::chrono::seconds>());
}

TEST_CASE("Chrono time points serialize as milliseconds since epoch", "[chrono][json]")
{
  auto const time_point = std::chrono::system_clock::time_point(std::chrono::milliseconds(1'234));
  auto const json = nlohmann::json(time_point);

  CHECK(json == nlohmann::json({{"since_epoch", 1'234}, {"unit", "milliseconds"}}));
  CHECK(json.get<std::chrono::system_clock::time_point>() == time_point);
  CHECK_THROWS(
    nlohmann::json({{"unit", "milliseconds"}}).get<std::chrono::system_clock::time_point>()
  );
}
