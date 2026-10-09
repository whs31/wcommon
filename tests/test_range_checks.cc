#include <radar/mms/common/angle.h>
#include <radar/mms/common/concepts.h>
#include <radar/mms/common/distance.h>
#include <radar/mms/common/math.h>
#include <radar/mms/common/vector3.h>
#include <radar/mms/common/velocity.h>

#include <limits>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("Quantity concept accepts RCommon quantities only", "[concepts][math]")
{
  STATIC_CHECK(Quantity<Angle>);
  STATIC_CHECK(Quantity<Distance>);
  STATIC_CHECK(Quantity<Velocity const&>);
  STATIC_CHECK(not Quantity<double>);
  STATIC_CHECK(not Quantity<Vector3>);

  STATIC_CHECK(QuantityTraits<Angle>::si_symbol == "rad");
  STATIC_CHECK(QuantityTraits<Distance>::si_symbol == "m");
  STATIC_CHECK(QuantityTraits<Velocity>::si_symbol == "m/s");
  STATIC_CHECK(QuantityTraits<Distance>::from_si(2.5) == Distance::from_meters(2.5));
}

TEMPLATE_TEST_CASE("Range checks accept finite values only", "[math]", float, double)
{
  using T = TestType;

  constexpr auto nan = std::numeric_limits<T>::quiet_NaN();
  constexpr auto inf = std::numeric_limits<T>::infinity();

  STATIC_CHECK(is_finite(T {-1}));
  STATIC_CHECK(is_finite(std::numeric_limits<T>::max()));
  STATIC_CHECK(is_finite(std::numeric_limits<T>::denorm_min()));
  STATIC_CHECK_FALSE(is_finite(inf));
  STATIC_CHECK_FALSE(is_finite(-inf));
  STATIC_CHECK_FALSE(is_finite(nan));

  STATIC_CHECK(positive(T {1}));
  STATIC_CHECK_FALSE(positive(T {0}));
  STATIC_CHECK_FALSE(positive(inf));
  STATIC_CHECK_FALSE(positive(nan));

  STATIC_CHECK(nonnegative(T {0}));
  STATIC_CHECK_FALSE(nonnegative(T {-1}));
  STATIC_CHECK_FALSE(nonnegative(inf));
  STATIC_CHECK_FALSE(nonnegative(nan));

  // bounds of another type convert to the checked one
  STATIC_CHECK(in_closed_range(T {0}, 0., 1.));
  STATIC_CHECK(in_closed_range(T {1}, 0.F, 1.F));
  STATIC_CHECK_FALSE(in_closed_range(T {1.5}, 0, 1));
  STATIC_CHECK_FALSE(in_closed_range(nan, -inf, inf));
}

TEST_CASE("Range checks take quantities by their SI magnitude", "[math]")
{
  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
  constexpr auto inf = std::numeric_limits<double>::infinity();

  STATIC_CHECK(is_finite(Distance::from_meters(-3.0)));
  STATIC_CHECK_FALSE(is_finite(Distance::from_meters(inf)));
  STATIC_CHECK_FALSE(is_finite(Angle::from_radians(nan)));

  STATIC_CHECK(positive(Distance::from_meters(0.5)));
  STATIC_CHECK_FALSE(positive(Distance::from_meters(0.0)));
  STATIC_CHECK_FALSE(positive(Distance::from_meters(nan)));

  STATIC_CHECK(nonnegative(Velocity::from_mps(0.0)));
  STATIC_CHECK_FALSE(nonnegative(Velocity::from_mps(-1.0)));
  STATIC_CHECK_FALSE(nonnegative(Velocity::from_mps(inf)));

  STATIC_CHECK(in_closed_range(45_deg, 0_deg, 90_deg));
  STATIC_CHECK(in_closed_range(90_deg, 0_deg, 90_deg));
  STATIC_CHECK_FALSE(in_closed_range(91_deg, 0_deg, 90_deg));
  STATIC_CHECK_FALSE(in_closed_range(Angle::from_radians(nan), -180_deg, 180_deg));
}

TEST_CASE("Quantization rounds to the nearest step", "[math]")
{
  CHECK(quantized(1.24, 0.5) == 1.0);
  CHECK(quantized(1.26, 0.5) == 1.5);
  CHECK(quantized(-1.26, 0.5) == -1.5);
  CHECK(quantized(1.26, 0.0) == 1.26);
  CHECK(quantized(1.26F, -1.0) == 1.26F);
}
