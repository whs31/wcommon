#include <radar/mms/common/angle.h>
#include <radar/mms/common/distance.h>
#include <radar/mms/common/euler_angles.h>
#include <radar/mms/common/math.h>
#include <radar/mms/common/velocity.h>

#include <array>
#include <limits>
#include <numbers>
#include <numeric>
#include <sstream>
#include <type_traits>
#include <unordered_set>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms::common;

namespace
{
  template <typename T>
  concept SelfMultipliable = requires(T value) { value * value; };
}  // namespace

TEST_CASE("Measurement values support constexpr construction and arithmetic", "[measurements]")
{
  STATIC_CHECK(Angle::zero().radians() == 0.0F);
  STATIC_CHECK(approx_eq(Angle::from_degrees(180.0F).radians(), std::numbers::pi_v<float>));
  STATIC_CHECK(Angle::tau().radians() == std::numbers::pi_v<float> * 2.0F);
  STATIC_CHECK(approx_eq((90.0_deg).radians(), std::numbers::pi_v<float> / 2.0F, 1e-4F));
  STATIC_CHECK((Angle::from_radians(2.0F) + Angle::from_radians(3.0F)).radians() == 5.0F);

  STATIC_CHECK(Distance::from_meters(2.0F).as_cm() == 200.0F);
  STATIC_CHECK(approx_eq((1.5_kilometers).as_m(), 1'500.0F, 1e-3F));
  STATIC_CHECK((Distance::from_meters(2.0F) + Distance::from_meters(3.0F)).as_m() == 5.0F);

  STATIC_CHECK(approx_eq(Velocity::from_kmph(36.0F).as_mps(), 10.0F, 1e-3F));
  STATIC_CHECK(approx_eq((1_mph).as_mps(), 1'609.344F / 3'600.0F, 1e-6F));
  STATIC_CHECK((Velocity::from_mps(2.0F) + Velocity::from_mps(3.0F)).as_mps() == 5.0F);

  STATIC_CHECK(EulerAngles::from_degrees(10.0F, 20.0F, 30.0F).yaw_degrees() == 30.0F);
  STATIC_CHECK(std::is_copy_constructible_v<EulerAngles>);
}

TEST_CASE("Measurement scaling only accepts dimensionless factors", "[measurements]")
{
  STATIC_CHECK(requires { Distance::from_meters(2.0F) * 3.0F; });
  STATIC_CHECK(requires { 3.0F * Distance::from_meters(2.0F); });
  STATIC_CHECK(requires { Distance::from_meters(2.0F) / 3.0F; });
  STATIC_CHECK_FALSE(SelfMultipliable<Distance>);
  STATIC_CHECK_FALSE(SelfMultipliable<Velocity>);
  STATIC_CHECK_FALSE(SelfMultipliable<Angle>);
}

TEST_CASE("Angle interpolation preserves full floating-point precision", "[measurements][angle]")
{
  auto const result = Angle::zero().lerp(Angle::from_radians(1.0F), 0.123456F);
  REQUIRE(result.radians() == 0.123456F);
}

TEST_CASE("Miles per hour conversions use the international mile", "[measurements][velocity]")
{
  auto const highway_speed = Velocity::from_mph(60.0F);
  REQUIRE(approx_eq(highway_speed.as_mps(), 26.8224F, 1e-5F));
  REQUIRE(approx_eq(highway_speed.as_mph(), 60.0F, 1e-5F));
}

TEST_CASE("Angle wrapping and trigonometry preserve the old contract", "[measurements][angle]")
{
  auto const heading = Angle::from_degrees(350.0F);
  auto const target = Angle::from_degrees(10.0F);

  REQUIRE(approx_eq(Angle::from_degrees(-10.0F).positive().degrees(), 350.0F, 1e-4F));
  REQUIRE(approx_eq(Angle::from_degrees(270.0F).signed_().degrees(), -90.0F, 1e-4F));
  REQUIRE(approx_eq(heading.angle_to(target).degrees(), 20.0F, 1e-4F));
  REQUIRE(approx_eq(target.angle_to(heading).degrees(), -20.0F, 1e-4F));
  REQUIRE(approx_eq(Angle::frac_pi_2().sin(), 1.0F, 1e-6F));
  REQUIRE_FALSE(Angle::from_radians(std::numeric_limits<float>::infinity()).is_finite());
  REQUIRE_FALSE(Angle::from_radians(std::numeric_limits<float>::quiet_NaN()).is_finite());
}

TEST_CASE("Distances and velocities convert and format", "[measurements]")
{
  REQUIRE(approx_eq(Distance::from_inches(12.0F).as_feet(), 1.0F, 1e-3F));
  REQUIRE(approx_eq(Distance::from_yards(1'760.0F).as_miles(), 1.0F, 1e-3F));
  REQUIRE(Distance::from_meters(1'500.0F).to_string() == "1.5 km");
  REQUIRE(Distance::from_meters(-1'500.0F).to_string() == "-1.5 km");
  REQUIRE(Velocity::from_mps(25.0F).to_string() == "25 m/s");

  auto distances = std::array {Distance::from_meters(1.0F), Distance::from_meters(2.0F)};
  REQUIRE(
    std::accumulate(distances.begin(), distances.end(), Distance()) == Distance::from_meters(3.0F)
  );

  auto stream = std::ostringstream();
  stream << Angle::from_degrees(45.0F);
  REQUIRE(stream.str() == "45 deg");
  REQUIRE(fmt::format("{}", Velocity::from_mps(25.0F)) == "25 m/s");

  auto values = std::unordered_set<Distance> {Distance::from_meters(3.0F)};
  REQUIRE(values.contains(Distance::from_meters(3.0F)));
}

TEST_CASE("Euler angles expose all components and integrations", "[measurements][euler]")
{
  constexpr auto angles = EulerAngles::from_degrees(10.0F, 20.0F, 30.0F);
  REQUIRE(angles.is_finite());
  REQUIRE(angles.to_string() == "roll 10 deg, pitch 20 deg, yaw 30 deg");
  REQUIRE(fmt::format("{}", angles) == angles.to_string());

  auto values = std::unordered_set<EulerAngles> {angles};
  REQUIRE(values.contains(angles));
}
