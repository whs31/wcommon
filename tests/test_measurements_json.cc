#include <radar/mms/common/json/angle.h>
#include <radar/mms/common/json/distance.h>
#include <radar/mms/common/json/euler_angles.h>
#include <radar/mms/common/json/velocity.h>

#include <numbers>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms::common;

TEST_CASE("Measurement JSON adapters preserve their schemas", "[measurements][json]")
{
  auto const angle = nlohmann::json(Angle::from_degrees(90.0F));
  REQUIRE(angle == nlohmann::json({{"angle", 90.0F}, {"units", "degrees"}}));
  REQUIRE(angle.get<Angle>() == Angle::from_degrees(90.0F));
  REQUIRE(
    nlohmann::json({{"angle", std::numbers::pi_v<float>}, {"units", "radians"}}).get<Angle>()
    == Angle::pi()
  );

  auto const distance = nlohmann::json(Distance::from_meters(12.5F));
  REQUIRE(distance == nlohmann::json({{"meters", 12.5F}}));
  REQUIRE(distance.get<Distance>() == Distance::from_meters(12.5F));

  auto const velocity = nlohmann::json(Velocity::from_mps(7.25F));
  REQUIRE(velocity == nlohmann::json({{"mps", 7.25F}}));
  REQUIRE(velocity.get<Velocity>() == Velocity::from_mps(7.25F));

  constexpr auto euler = EulerAngles::from_degrees(10.0F, 20.0F, 30.0F);
  auto const euler_json = nlohmann::json(euler);
  REQUIRE(euler_json.get<EulerAngles>() == euler);
}

TEST_CASE("Measurement JSON adapters reject invalid schemas", "[measurements][json]")
{
  REQUIRE_THROWS(nlohmann::json({{"angle", 90.0F}, {"units", "turns"}}).get<Angle>());
  REQUIRE_THROWS(nlohmann::json({{"angle", "90"}, {"units", "degrees"}}).get<Angle>());
  REQUIRE_THROWS(nlohmann::json({{"meters", "12.5"}}).get<Distance>());
  REQUIRE_THROWS(nlohmann::json({{"mps", nullptr}}).get<Velocity>());
  REQUIRE_THROWS(nlohmann::json({{"roll", 1}, {"pitch", 2}}).get<EulerAngles>());
}
