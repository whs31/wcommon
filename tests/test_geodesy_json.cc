#include <radar/mms/common/geodesy.h>
#include <radar/mms/common/json/geodesy.h>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("Geodetic position JSON adapter round-trips its quantities", "[geodesy][json]")
{
  auto constexpr position = GeodeticPosition {
    .latitude = 45_deg,
    .longitude = -30.5_deg,
    .altitude_msl = 120_meters,
  };
  auto const json = nlohmann::json(position);

  REQUIRE(
    json
    == nlohmann::json::parse(
      R"({"latitude": {"angle": 45.0, "units": "degrees"},
          "longitude": {"angle": -30.5, "units": "degrees"},
          "altitude_msl": {"meters": 120.0}})"
    )
  );
  REQUIRE(json.get<GeodeticPosition>() == position);
}

TEST_CASE("Geodetic position JSON adapter rejects missing members", "[geodesy][json]")
{
  REQUIRE_THROWS(
    nlohmann::json::parse(R"({"latitude": {"angle": 45.0, "units": "degrees"},
                              "longitude": {"angle": -30.5, "units": "degrees"}})")
      .get<GeodeticPosition>()
  );
}
