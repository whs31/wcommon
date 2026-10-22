#include <radar/mms/common/geodesy.h>
#include <radar/mms/common/glaze/angle.h>
#include <radar/mms/common/glaze/distance.h>

#include <string>

#include <glaze/json.hpp>
#include <glaze/toml.hpp>

#include <catch2/catch_test_macros.hpp>

#include "glaze_support.h"

using namespace radar::mms;
using namespace glaze_support;

TEST_CASE("Geodetic positions glaze-reflect into their quantities", "[glaze][geodesy]")
{
  check_round_trip(
    GeodeticPosition {
      .latitude = 45_deg,
      .longitude = -30.5_deg,
      .altitude_msl = 120_meters,
    },
    R"({"latitude":{"angle":45,"units":"degrees"},"longitude":{"angle":-30.5,"units":"degrees"},"altitude_msl":{"meters":120}})"
  );
  REQUIRE(
    strict_read_error<GeodeticPosition>(R"({"latitude":{"degrees":60},"longitude":{"degrees":30}})")
    == glz::error_code::missing_key
  );
}

TEST_CASE("Geodetic positions read from TOML in natural units", "[glaze][geodesy][toml]")
{
  auto position = GeodeticPosition {};
  auto const toml = std::string(R"(latitude = "59.771deg"
longitude = { degrees = 30.326 }
altitude_msl = "70m"
)");
  auto const error = glz::read_toml(position, toml);
  INFO(glz::format_error(error, toml));
  REQUIRE_FALSE(error);
  REQUIRE(
    position
    == GeodeticPosition {
      .latitude = 59.771_deg,
      .longitude = 30.326_deg,
      .altitude_msl = 70_meters,
    }
  );
}
