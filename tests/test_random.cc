#include <radar/mms/common/error.h>
#include <radar/mms/common/random.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <array>
#include <bit>
#include <limits>
#include <tuple>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

namespace
{
  auto constexpr world_seed = std::uint64_t {1};

  // distance in units in the last place between two finite doubles of the same sign
  [[nodiscard]] std::int64_t ulps_between(double const a, double const b)
  {
    return std::abs(std::bit_cast<std::int64_t>(a) - std::bit_cast<std::int64_t>(b));
  }
}  // namespace

TEST_CASE("Random stream is a trivially copyable aggregate", "[random]")
{
  STATIC_CHECK(std::is_trivially_copyable_v<RandomStream>);
  STATIC_CHECK(std::is_aggregate_v<RandomStream>);
  STATIC_CHECK(sizeof(RandomStream) == 32);
  STATIC_CHECK(RandomStream() == RandomStream::from_seed(0));
}

TEST_CASE("SplitMix64 matches its reference implementation", "[random]")
{
  auto constexpr expected = std::array<std::uint64_t, 5> {
    6'457'827'717'110'365'317U,
    3'203'168'211'198'807'973U,
    9'817'491'932'198'370'423U,
    4'593'380'528'125'082'431U,
    16'408'922'859'458'223'821U,
  };
  auto state = std::uint64_t {1'234'567};
  for(auto const value : expected)
    CHECK(_random_impl::splitmix64(state) == value);
}

TEST_CASE("xoshiro256** matches its reference implementation", "[random]")
{
  auto constexpr expected = std::array<std::uint64_t, 6> {
    11'520U,
    0U,
    1'509'978'240U,
    1'215'971'899'390'074'240U,
    1'216'172'134'540'287'360U,
    607'988'272'756'665'600U,
  };
  auto stream = RandomStream {.state = {1, 2, 3, 4}};
  for(auto const value : expected)
    CHECK(stream.next() == value);
}

TEST_CASE("Golden values of a known stream are identical on every platform", "[random]")
{
  // computed by an independent Python implementation of the same algorithms; a mismatch means the
  // platform rounds differently (e.g. contracted multiply-adds), not that the values need updating
  STATIC_CHECK(stream_seed(world_seed, "vt30-1", "imu.gyro") == 0x3cd2'a1fd'17aa'786fU);

  auto uniforms = RandomStream::derive(world_seed, "vt30-1", "imu.gyro");

  CHECK(uniforms.uniform() == 0x1.fb2009f60d3d8p-2);
  CHECK(uniforms.uniform() == 0x1.437b059a7e7ecp-2);
  CHECK(uniforms.uniform() == 0x1.8cdc0f24a9e8cp-2);

  auto constexpr expected = std::array {
    -0x1.a744e8a1d9995p-5,
    -0x1.2e70c65f89a6ap+1,
    0x1.126dbbe26383fp-6,
    -0x1.71b2175befc99p-1,
    -0x1.5d9b864dc4360p-1,
    -0x1.54e8e9556750ap-2,
    0x1.0e648de5feb94p-2,
    0x1.f96886155cccdp-5,
    -0x1.b1e3617486ceap-1,
    -0x1.846bdb00c3f75p-1,
  };
  auto normals = RandomStream::derive(world_seed, "vt30-1", "imu.gyro");
  for(auto const value : expected)
    CHECK(normals.normal() == value);
}

TEST_CASE("Stream seeds depend on the seed, scope and stream only", "[random]")
{
  auto constexpr gyro = stream_seed(world_seed, "vt30-1", "imu.gyro");

  STATIC_CHECK(gyro == stream_seed(world_seed, "vt30-1", "imu.gyro"));
  STATIC_CHECK(gyro != stream_seed(world_seed + 1, "vt30-1", "imu.gyro"));
  STATIC_CHECK(gyro != stream_seed(world_seed, "vt30-2", "imu.gyro"));
  STATIC_CHECK(gyro != stream_seed(world_seed, "vt30-1", "imu.accel"));
  STATIC_CHECK(gyro != stream_seed(world_seed, "imu.gyro", "vt30-1"));
  STATIC_CHECK(stream_seed(world_seed, "ab", "c") != stream_seed(world_seed, "a", "bc"));
  STATIC_CHECK(stream_seed(world_seed, "", "") != stream_seed(world_seed + 1, "", ""));
  STATIC_CHECK(
    RandomStream::derive(world_seed, "vt30-1", "imu.gyro") == RandomStream::from_seed(gyro)
  );
}

TEST_CASE("A copied stream forks into two identical sequences", "[random]")
{
  auto original = RandomStream::derive(world_seed, "vt30-1", "servo.1");
  std::ignore = original.next();
  auto copy = original;

  for(auto draw = 0; draw < 100; ++draw)
    REQUIRE(original.normal() == copy.normal());
  CHECK(original == copy);
}

TEST_CASE("Uniform samples stay in their interval", "[random]")
{
  auto stream = RandomStream::derive(world_seed, "vt30-1", "uniform");
  auto sum = 0.0;
  auto constexpr count = 100'000;
  for(auto draw = 0; draw < count; ++draw) {
    auto const value = stream.uniform();

    REQUIRE(value >= 0.0);
    REQUIRE(value < 1.0);
    sum += value;

    auto const scaled = stream.uniform(-2.0, 3.0);

    REQUIRE(scaled >= -2.0);
    REQUIRE(scaled <= 3.0);
  }
  CHECK(std::abs(sum / count - 0.5) < 0.005);
  CHECK(RandomStream {.state = {0, 0, 0, 0}}.uniform() == 0.0);
}

TEST_CASE("Whole numbers below a bound are uniform", "[random]")
{
  auto stream = RandomStream::derive(world_seed, "vt30-1", "offset");
  auto counts = std::array<int, 6> {};
  for(auto draw = 0; draw < 60'000; ++draw) {
    auto const value = stream.below(counts.size());

    REQUIRE(value < counts.size());
    ++counts.at(value);
  }
  for(auto const count : counts)
    CHECK(std::abs(count - 10'000) < 500);

  auto constexpr huge = (std::uint64_t {1} << 63U) + 1U;
  for(auto draw = 0; draw < 1'000; ++draw)
    REQUIRE(stream.below(huge) < huge);
  CHECK(stream.below(1) == 0);
  CHECK_THROWS_AS(stream.below(0), InvalidArgumentError);
}

TEST_CASE("Normal samples have the requested mean and deviation", "[random]")
{
  auto stream = RandomStream::derive(world_seed, "vt30-1", "normal");
  auto constexpr count = 200'000;
  auto sum = 0.0;
  auto sum_of_squares = 0.0;
  auto beyond_three_sigma = 0;
  for(auto draw = 0; draw < count; ++draw) {
    auto const value = stream.normal();

    REQUIRE(std::isfinite(value));

    sum += value;
    sum_of_squares += value * value;
    beyond_three_sigma += std::abs(value) > 3.0 ? 1 : 0;
  }
  auto const mean = sum / count;

  CHECK(std::abs(mean) < 0.01);
  CHECK(std::abs(sum_of_squares / count - mean * mean - 1.0) < 0.015);
  CHECK(std::abs(beyond_three_sigma - 540) < 100);

  auto scaled = RandomStream::derive(world_seed, "vt30-1", "normal");
  auto plain = scaled;
  for(auto draw = 0; draw < 100; ++draw)
    CHECK(scaled.normal(10.0, 0.5) == 10.0 + 0.5 * plain.normal());
}

TEST_CASE("Portable logarithm stays within one ulp of the C runtime", "[random]")
{
  CHECK(portable_log(1.0) == 0.0);
  CHECK(portable_log(2.0) == std::log(2.0));

  auto worst_ulps = std::int64_t {0};
  auto worst_x = 0.0;
  auto const check = [&worst_ulps, &worst_x](double const x) {
    auto const ulps = ulps_between(portable_log(x), std::log(x));
    if(ulps > worst_ulps) {
      worst_ulps = ulps;
      worst_x = x;
    }
  };
  check(std::numeric_limits<double>::denorm_min());
  check(std::numeric_limits<double>::min());
  check(std::numeric_limits<double>::max());
  check(0x1.6a09e667f3bcdp-1);
  check(0x1.6a09e667f3bccp-1);
  auto stream = RandomStream::derive(world_seed, "test", "log");
  for(auto draw = 0; draw < 100'000; ++draw) {
    if(auto const fraction = stream.uniform(); fraction > 0.0)
      check(fraction);
    check(std::ldexp(0.5 + stream.uniform(), static_cast<int>(stream.below(2'000)) - 1'000));
  }

  INFO("worst at x = " << worst_x);
  CHECK(worst_ulps <= 1);
}

TEST_CASE("Portable exponential stays within one ulp of the C runtime", "[random]")
{
  CHECK(portable_exp(0.0) == 1.0);

  auto worst_ulps = std::int64_t {0};
  auto worst_x = 0.0;
  auto const check = [&worst_ulps, &worst_x](double const x) {
    auto const ulps = ulps_between(portable_exp(x), std::exp(x));
    if(ulps > worst_ulps) {
      worst_ulps = ulps;
      worst_x = x;
    }
  };
  check(1.0);
  check(-708.0);
  check(709.0);
  check(1e-300);
  check(-1e-300);
  // around the reduction boundaries ±½ ln 2
  check(0x1.62e42fefa39efp-2);
  check(-0x1.62e42fefa39efp-2);
  auto stream = RandomStream::derive(world_seed, "test", "exp");
  for(auto draw = 0; draw < 100'000; ++draw) {
    check(stream.uniform(-708.0, 709.0));
    // the exponents of the Gauss–Markov processes, −Tₛ/τ
    check(-stream.uniform());
  }

  INFO("worst at x = " << worst_x);
  CHECK(worst_ulps <= 1);
}
