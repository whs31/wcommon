#include <radar/mms/common/angle.h>
#include <radar/mms/common/curve.h>
#include <radar/mms/common/distance.h>
#include <radar/mms/common/error.h>

#include <array>
#include <limits>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("Curve stays trivially copyable for embedding into state", "[curve]")
{
  STATIC_CHECK(std::is_trivially_copyable_v<Curve<double, double, 4>>);
  STATIC_CHECK(std::is_trivially_copyable_v<Curve<Angle, Distance, 4>>);
}

TEST_CASE("Default-constructed curve is empty and cannot be interpolated", "[curve]")
{
  constexpr auto c = Curve<double, double, 4>();

  STATIC_CHECK(c.empty());
  STATIC_CHECK(c.arguments().empty());
  STATIC_CHECK(c.values().empty());
  CHECK_THROWS_AS(c.interpolate(0.0), InvalidArgumentError);
}

TEST_CASE("Curve preserves its points in order", "[curve]")
{
  auto const
    c = Curve<double, double, 4>(std::array {0.0, 0.5, 1.0}, std::array {10.0, 20.0, 30.0});

  REQUIRE(c.size() == 3);
  CHECK(std::vector(c.arguments().begin(), c.arguments().end()) == std::vector {0.0, 0.5, 1.0});
  CHECK(std::vector(c.values().begin(), c.values().end()) == std::vector {10.0, 20.0, 30.0});
  CHECK(c == Curve<double, double, 4>(std::array {0.0, 0.5, 1.0}, std::array {10.0, 20.0, 30.0}));
  CHECK_FALSE(c == Curve<double, double, 4>(std::array {0.0, 0.5}, std::array {10.0, 20.0}));
}

TEST_CASE("Curve interpolates linearly and clamps outside its range", "[curve]")
{
  constexpr auto arguments = std::array {0.0, 0.5, 1.0};
  constexpr auto values = std::array {10.0, 20.0, 30.0};
  constexpr auto c = Curve<double, double, 4>(arguments, values);

  STATIC_CHECK(c.interpolate(0.25) == 15.0);
  STATIC_CHECK(c.interpolate(0.5) == 20.0);
  STATIC_CHECK(c.interpolate(-1.0) == 10.0);
  STATIC_CHECK(c.interpolate(2.0) == 30.0);
  CHECK_THROWS_AS(c.interpolate(std::numeric_limits<double>::quiet_NaN()), InvalidArgumentError);
  CHECK_THROWS_AS(c.interpolate(std::numeric_limits<double>::infinity()), InvalidArgumentError);
}

TEST_CASE("Single-point curve is constant", "[curve]")
{
  auto const c = Curve<double, double, 4>(std::array {1.0}, std::array {5.0});

  CHECK(c.interpolate(0.0) == 5.0);
  CHECK(c.interpolate(3.0) == 5.0);
}

TEST_CASE("Curve rejects malformed point sets on construction", "[curve]")
{
  using C = Curve<double, double, 4>;

  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();

  CHECK_THROWS_AS(C(std::array {0.0, 1.0}, std::array {1.0}), InvalidArgumentError);
  CHECK_THROWS_AS(C(std::vector<double> {}, std::vector<double> {}), InvalidArgumentError);
  CHECK_THROWS_AS(
    C(std::array {0.0, 1.0, 2.0, 3.0, 4.0}, std::array {0.0, 1.0, 2.0, 3.0, 4.0}),
    InvalidArgumentError
  );
  CHECK_THROWS_AS(C(std::array {0.5, 0.5}, std::array {1.0, 2.0}), InvalidArgumentError);
  CHECK_THROWS_AS(C(std::array {1.0, 0.0}, std::array {1.0, 2.0}), InvalidArgumentError);
  CHECK_THROWS_AS(C(std::array {0.0, nan}, std::array {1.0, 2.0}), InvalidArgumentError);
  CHECK_THROWS_AS(C(std::array {0.0, 1.0}, std::array {1.0, nan}), InvalidArgumentError);
}

TEST_CASE("Curve interpolates values in another precision than its arguments", "[curve]")
{
  auto const c = Curve<double, float, 2>(std::array {0.0, 1.0}, std::array {10.0F, 20.0F});

  STATIC_CHECK(std::is_same_v<decltype(c.interpolate(0.5)), float>);
  CHECK(c.interpolate(0.25) == 12.5F);
}

TEST_CASE("Curve axes may be quantities", "[curve]")
{
  constexpr auto arguments = std::array {0_deg, 90_deg};
  constexpr auto values = std::array {Distance::from_meters(1.0), Distance::from_meters(3.0)};
  constexpr auto c = Curve<Angle, Distance, 2>(arguments, values);

  STATIC_CHECK(c.interpolate(Angle::from_radians(0.0)) == Distance::from_meters(1.0));
  CHECK(c.interpolate(45_deg).as_m() == 2.0);
  CHECK(c.interpolate(180_deg).as_m() == 3.0);
  CHECK_THROWS_AS(
    (Curve<Angle, double, 2>(std::array {10_deg, 5_deg}, std::array {0.0, 1.0})),
    InvalidArgumentError
  );
}
