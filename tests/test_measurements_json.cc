#include <whs31/common/json/angle.h>
#include <whs31/common/json/distance.h>
#include <whs31/common/json/euler_angles.h>
#include <whs31/common/json/velocity.h>

#include <numbers>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace whs31::common;

TEST_CASE("Measurement JSON adapters preserve their schemas", "[measurements][json]")
{
  auto const angle = nlohmann::json(Angle::from_degrees(90.0));
  REQUIRE(angle == nlohmann::json({{"angle", 90.0}, {"units", "degrees"}}));
  REQUIRE(angle.get<Angle>() == Angle::from_degrees(90.0));
  REQUIRE(
    nlohmann::json({{"angle", std::numbers::pi_v<double>}, {"units", "radians"}}).get<Angle>()
    == Angle::pi()
  );

  auto const distance = nlohmann::json(Distance::from_meters(12.5));
  REQUIRE(distance == nlohmann::json({{"meters", 12.5}}));
  REQUIRE(distance.get<Distance>() == Distance::from_meters(12.5));

  auto const velocity = nlohmann::json(Velocity::from_mps(7.25));
  REQUIRE(velocity == nlohmann::json({{"mps", 7.25}}));
  REQUIRE(velocity.get<Velocity>() == Velocity::from_mps(7.25));

  auto constexpr euler = EulerAngles::from_degrees(10.0, 20.0, 30.0);
  auto const euler_json = nlohmann::json(euler);
  REQUIRE(euler_json.get<EulerAngles>() == euler);
}

TEST_CASE("Measurement JSON adapters reject invalid schemas", "[measurements][json]")
{
  REQUIRE_THROWS(nlohmann::json({{"angle", 90.0}, {"units", "turns"}}).get<Angle>());
  REQUIRE_THROWS(nlohmann::json({{"angle", "90"}, {"units", "degrees"}}).get<Angle>());
  REQUIRE_THROWS(nlohmann::json({{"meters", "12.5"}}).get<Distance>());
  REQUIRE_THROWS(nlohmann::json({{"mps", nullptr}}).get<Velocity>());
  REQUIRE_THROWS(nlohmann::json({{"roll", 1}, {"pitch", 2}}).get<EulerAngles>());
}
