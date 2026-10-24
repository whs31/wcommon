#include <whs31/common/angle.h>
#include <whs31/common/concepts.h>
#include <whs31/common/distance.h>
#include <whs31/common/math.h>
#include <whs31/common/vector3.h>
#include <whs31/common/velocity.h>

#include <limits>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace whs31;

TEST_CASE("Quantity concept accepts WCommon quantities only", "[concepts][math]")
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

  STATIC_CHECK(is_finite(T {-1}));
  STATIC_CHECK(is_finite(std::numeric_limits<T>::max()));
  STATIC_CHECK(is_finite(std::numeric_limits<T>::denorm_min()));
  STATIC_CHECK_FALSE(is_finite(Inf));
  STATIC_CHECK_FALSE(is_finite(-Inf));
  STATIC_CHECK_FALSE(is_finite(NaN));

  STATIC_CHECK(positive(T {1}));
  STATIC_CHECK_FALSE(positive(T {0}));
  STATIC_CHECK_FALSE(positive(Inf));
  STATIC_CHECK_FALSE(positive(NaN));

  STATIC_CHECK(nonnegative(T {0}));
  STATIC_CHECK_FALSE(nonnegative(T {-1}));
  STATIC_CHECK_FALSE(nonnegative(Inf));
  STATIC_CHECK_FALSE(nonnegative(NaN));

  // bounds of another type convert to the checked one
  STATIC_CHECK(in_closed_range(T {0}, 0., 1.));
  STATIC_CHECK(in_closed_range(T {1}, 0.F, 1.F));
  STATIC_CHECK_FALSE(in_closed_range(T {1.5}, 0, 1));
  STATIC_CHECK_FALSE(in_closed_range(NaN, -Inf, Inf));
}

TEST_CASE("Range checks take quantities by their SI magnitude", "[math]")
{
  STATIC_CHECK(is_finite(Distance::from_meters(-3.0)));
  STATIC_CHECK_FALSE(is_finite(Distance::from_meters(Inf)));
  STATIC_CHECK_FALSE(is_finite(Angle::from_radians(NaN)));

  STATIC_CHECK(positive(Distance::from_meters(0.5)));
  STATIC_CHECK_FALSE(positive(Distance::from_meters(0.0)));
  STATIC_CHECK_FALSE(positive(Distance::from_meters(NaN)));

  STATIC_CHECK(nonnegative(Velocity::from_mps(0.0)));
  STATIC_CHECK_FALSE(nonnegative(Velocity::from_mps(-1.0)));
  STATIC_CHECK_FALSE(nonnegative(Velocity::from_mps(Inf)));

  STATIC_CHECK(in_closed_range(45_deg, 0_deg, 90_deg));
  STATIC_CHECK(in_closed_range(90_deg, 0_deg, 90_deg));
  STATIC_CHECK_FALSE(in_closed_range(91_deg, 0_deg, 90_deg));
  STATIC_CHECK_FALSE(in_closed_range(Angle::from_radians(NaN), -180_deg, 180_deg));
}

TEST_CASE("Quantization rounds to the nearest step", "[math]")
{
  CHECK(quantized(1.24, 0.5) == 1.0);
  CHECK(quantized(1.26, 0.5) == 1.5);
  CHECK(quantized(-1.26, 0.5) == -1.5);
  CHECK(quantized(1.26, 0.0) == 1.26);
  CHECK(quantized(1.26F, -1.0) == 1.26F);
}
