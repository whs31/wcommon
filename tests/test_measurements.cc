#include <whs31/common/angle.h>
#include <whs31/common/distance.h>
#include <whs31/common/euler_angles.h>
#include <whs31/common/math.h>
#include <whs31/common/velocity.h>

#include <array>
#include <limits>
#include <numbers>
#include <numeric>
#include <sstream>
#include <type_traits>
#include <unordered_set>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

using namespace whs31::common;

namespace
{
  template <typename T>
  concept SelfMultipliable = requires(T value) { value * value; };
}  // namespace

TEST_CASE("Measurement values support constexpr construction and arithmetic", "[measurements]")
{
  STATIC_CHECK(Angle::zero().radians() == 0.0);
  STATIC_CHECK(approx_eq(Angle::from_degrees(180.0).radians(), std::numbers::pi_v<double>));
  STATIC_CHECK(Angle::tau().radians() == std::numbers::pi_v<double> * 2.0);
  STATIC_CHECK(approx_eq((90.0_deg).radians(), std::numbers::pi_v<double> / 2.0, 1e-4));
  STATIC_CHECK((Angle::from_radians(2.0) + Angle::from_radians(3.0)).radians() == 5.0);

  STATIC_CHECK(Distance::from_meters(2.0).as_cm() == 200.0);
  STATIC_CHECK(approx_eq((1.5_kilometers).as_m(), 1'500.0, 1e-3));
  STATIC_CHECK((Distance::from_meters(2.0) + Distance::from_meters(3.0)).as_m() == 5.0);

  STATIC_CHECK(approx_eq(Velocity::from_kmph(36.0).as_mps(), 10.0, 1e-3));
  STATIC_CHECK(approx_eq((1_mph).as_mps(), 1'609.344 / 3'600.0, 1e-6));
  STATIC_CHECK((Velocity::from_mps(2.0) + Velocity::from_mps(3.0)).as_mps() == 5.0);

  STATIC_CHECK(approx_eq(EulerAngles::from_degrees(10.0, 20.0, 30.0).yaw_degrees(), 30.0, 1e-12));
  STATIC_CHECK(std::is_copy_constructible_v<EulerAngles>);
}

TEST_CASE("Measurement scaling only accepts dimensionless factors", "[measurements]")
{
  STATIC_CHECK(requires { Distance::from_meters(2.0) * 3.0; });
  STATIC_CHECK(requires { 3.0 * Distance::from_meters(2.0); });
  STATIC_CHECK(requires { Distance::from_meters(2.0) / 3.0; });
  STATIC_CHECK_FALSE(SelfMultipliable<Distance>);
  STATIC_CHECK_FALSE(SelfMultipliable<Velocity>);
  STATIC_CHECK_FALSE(SelfMultipliable<Angle>);
}

TEST_CASE("Angle interpolation preserves full floating-point precision", "[measurements][angle]")
{
  auto const result = Angle::zero().lerp(Angle::from_radians(1.0), 0.123456);
  REQUIRE(result.radians() == 0.123456);
}

TEST_CASE("Miles per hour conversions use the international mile", "[measurements][velocity]")
{
  auto const highway_speed = Velocity::from_mph(60.0);
  REQUIRE(approx_eq(highway_speed.as_mps(), 26.8224, 1e-5));
  REQUIRE(approx_eq(highway_speed.as_mph(), 60.0, 1e-5));
}

TEST_CASE("Angle wrapping and trigonometry preserve the old contract", "[measurements][angle]")
{
  auto const heading = Angle::from_degrees(350.0);
  auto const target = Angle::from_degrees(10.0);

  REQUIRE(approx_eq(Angle::from_degrees(-10.0).positive().degrees(), 350.0, 1e-4));
  REQUIRE(approx_eq(Angle::from_degrees(270.0).signed_().degrees(), -90.0, 1e-4));
  REQUIRE(approx_eq(heading.angle_to(target).degrees(), 20.0, 1e-4));
  REQUIRE(approx_eq(target.angle_to(heading).degrees(), -20.0, 1e-4));
  REQUIRE(approx_eq(Angle::frac_pi_2().sin(), 1.0, 1e-6));
  REQUIRE_FALSE(Angle::from_radians(std::numeric_limits<double>::infinity()).is_finite());
  REQUIRE_FALSE(Angle::from_radians(std::numeric_limits<double>::quiet_NaN()).is_finite());
}

TEST_CASE("Distances and velocities convert and format", "[measurements]")
{
  REQUIRE(approx_eq(Distance::from_inches(12.0).as_feet(), 1.0, 1e-3));
  REQUIRE(approx_eq(Distance::from_yards(1'760.0).as_miles(), 1.0, 1e-3));
  REQUIRE(Distance::from_meters(1'500.0).to_string() == "1.5 km");
  REQUIRE(Distance::from_meters(-1'500.0).to_string() == "-1.5 km");
  REQUIRE(Velocity::from_mps(25.0).to_string() == "25 m/s");

  auto distances = std::array {Distance::from_meters(1.0), Distance::from_meters(2.0)};
  REQUIRE(
    std::accumulate(distances.begin(), distances.end(), Distance()) == Distance::from_meters(3.0)
  );

  auto stream = std::ostringstream();
  stream << Angle::from_degrees(45.0);
  REQUIRE(stream.str() == "45 deg");
  REQUIRE(fmt::format("{}", Velocity::from_mps(25.0)) == "25 m/s");

  auto values = std::unordered_set<Distance> {Distance::from_meters(3.0)};
  REQUIRE(values.contains(Distance::from_meters(3.0)));
}

TEST_CASE("Euler angles expose all components and integrations", "[measurements][euler]")
{
  auto constexpr angles = EulerAngles::from_degrees(10.0, 20.0, 30.0);
  REQUIRE(angles.is_finite());
  REQUIRE(angles.to_string() == "roll 10 deg, pitch 20 deg, yaw 30 deg");
  REQUIRE(fmt::format("{}", angles) == angles.to_string());

  auto values = std::unordered_set<EulerAngles> {angles};
  REQUIRE(values.contains(angles));
}
