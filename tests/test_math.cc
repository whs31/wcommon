#include <whs31/common/constants.h>
#include <whs31/common/math.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

TEST_CASE("Measurement constants preserve their unit relationships", "[constants][math]")
{
  STATIC_CHECK(SECONDS_HOURS_FACTOR == 3'600.0);
  STATIC_CHECK(METER_MILLIMETER_FACTOR == 1'000.0);
  STATIC_CHECK(approx_eq(METER_FEET_FACTOR * 0.3048, 1.0, 1e-12));
  STATIC_CHECK(approx_eq(EARTH_RADIUS_KM * 1'000.0, EARTH_RADIUS, 1e-6));
}

TEST_CASE("floor supports constexpr values", "[math]")
{
  STATIC_CHECK(whs31::floor(3.75) == 3.0);
  STATIC_CHECK(whs31::floor(3.0) == 3.0);
  STATIC_CHECK(whs31::floor(-3.75) == -4.0);
  STATIC_CHECK(whs31::floor(-3.0) == -3.0);
}

TEST_CASE("floor preserves non-finite and large values", "[math]")
{
  STATIC_CHECK(whs31::floor(Inf) == Inf);
  STATIC_CHECK(std::isnan(whs31::floor(NaN)));
  STATIC_CHECK(
    whs31::floor(std::numeric_limits<double>::max()) == std::numeric_limits<double>::max()
  );
}

TEST_CASE("abs and fmod support constexpr values", "[math]")
{
  STATIC_CHECK(whs31::abs(-12) == 12);
  STATIC_CHECK(whs31::abs(12) == 12);
  STATIC_CHECK(whs31::fmod(5.5F, 2.0F) == 1.5F);
  STATIC_CHECK(whs31::fmod(-5.5F, 2.0F) == 0.5F);
  STATIC_CHECK(whs31::fmod(5.5F, -2.0F) == -0.5F);
}

TEST_CASE("Radians and degrees conversions support constexpr values", "[math]")
{
  STATIC_CHECK(approx_eq(to_radians(180.0), std::numbers::pi, 1e-12));
  STATIC_CHECK(approx_eq(to_degrees(std::numbers::pi), 180.0, 1e-12));
  STATIC_CHECK(to_radians(180) == 3);
  STATIC_CHECK(to_degrees(3) == 171);
}

TEST_CASE("Approximate comparisons support constexpr values", "[math]")
{
  STATIC_CHECK(approx_eq(42, 42));
  STATIC_CHECK(not approx_eq(42, 43));
  STATIC_CHECK(approx_eq(1.0, 1.0 + 1e-6, 1e-5));
  STATIC_CHECK(not approx_eq(1.0, 1.0 + 1e-4, 1e-5));
  STATIC_CHECK(is_null(1e-6, 1e-5));
  STATIC_CHECK(not is_null(1e-4, 1e-5));
}

TEST_CASE("Clamp, lerp and normalize support constexpr values", "[math]")
{
  STATIC_CHECK(clamp(5, 0, 10) == 5);
  STATIC_CHECK(clamp(-1, 0, 10) == 0);
  STATIC_CHECK(clamp(11, 0, 10) == 10);
  STATIC_CHECK(clamp(5, 10, 0) == 5);

  STATIC_CHECK(lerp(2.0, 10.0, 0.25) == 4.0);
  STATIC_CHECK(lerp(2, 10, 1) == 10);

  STATIC_CHECK(normalize(5.0, 0.0, 10.0) == 0.5);
  STATIC_CHECK(normalize(-5.0, 0.0, 10.0) == 0.0);
  STATIC_CHECK(normalize(15.0, 0.0, 10.0) == 1.0);
  STATIC_CHECK(normalize(5, 0, 10) == 0);
  STATIC_CHECK(normalize(10, 0, 10) == 1);
  STATIC_CHECK(normalize(5, 5, 5) == 0);
}

TEST_CASE("Euclidean division returns a nonnegative remainder", "[math]")
{
  struct Case
  {
    int a;
    int b;
    int quotient;
    int remainder;
  };

  for(auto const [a, b, quotient, remainder] : {
        Case {.a = 7, .b = 4, .quotient = 1, .remainder = 3},
        Case {.a = -7, .b = 4, .quotient = -2, .remainder = 1},
        Case {.a = 7, .b = -4, .quotient = -1, .remainder = 3},
        Case {.a = -7, .b = -4, .quotient = 2, .remainder = 1},
      }) {
    REQUIRE(div_euclid(a, b) == quotient);
    REQUIRE(rem_euclid(a, b) == remainder);
    REQUIRE(quotient * b + remainder == a);
  }
}

TEST_CASE("Euclidean division preserves large integer precision", "[math]")
{
  auto constexpr dividend = std::int64_t {18'014'398'509'481'987};
  auto constexpr divisor = std::int64_t {2};

  REQUIRE(div_euclid(dividend, divisor) == 9'007'199'254'740'993);
  REQUIRE(
    rem_euclid(std::int64_t {-1}, std::numeric_limits<std::int64_t>::min())
    == std::numeric_limits<std::int64_t>::max()
  );
}

TEST_CASE("Euclidean division supports floating point values", "[math]")
{
  auto const quotient = div_euclid(-7.5, 4.0);
  auto const remainder = rem_euclid(-7.5, 4.0);

  REQUIRE(quotient == -2.0);
  REQUIRE(remainder == 0.5);
  REQUIRE(approx_eq(quotient * 4.0 + remainder, -7.5, 1e-12));
}

TEST_CASE("Normalize handles reversed and degenerate ranges", "[math]")
{
  REQUIRE(normalize(5.0, 10.0, 0.0) == 0.5);
  REQUIRE(normalize(12.0, 10.0, 0.0) == 0.0);
  REQUIRE(std::isnan(normalize(1.0, 1.0, 1.0)));
  REQUIRE(normalize(1, 1, 1) == 0);
}
