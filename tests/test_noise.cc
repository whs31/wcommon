#include <radar/mms/common/error.h>
#include <radar/mms/common/noise.h>
#include <radar/mms/common/random.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <chrono>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace radar::mms;
using namespace std::chrono_literals;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

namespace
{
  constexpr auto world_seed = std::uint64_t {1};

  [[nodiscard]] double mean_of(std::vector<double> const& values)
  {
    auto sum = 0.0;
    for(auto const value : values)
      sum += value;
    return sum / static_cast<double>(values.size());
  }

  // the sample autocovariance at a lag of `lag` samples, about the known mean 0
  [[nodiscard]] double autocovariance(std::vector<double> const& values, std::size_t const lag)
  {
    auto const lagged = std::next(values.begin(), static_cast<std::ptrdiff_t>(lag));
    auto const sum = std::transform_reduce(lagged, values.end(), values.begin(), 0.0);
    return sum / static_cast<double>(values.size() - lag);
  }
}  // namespace

TEST_CASE("Noise processes are trivially copyable aggregates", "[random][noise]")
{
  STATIC_CHECK(std::is_trivially_copyable_v<WhiteNoise>);
  STATIC_CHECK(std::is_trivially_copyable_v<GaussMarkov>);
  STATIC_CHECK(std::is_trivially_copyable_v<RandomWalk>);
  STATIC_CHECK(std::is_aggregate_v<GaussMarkov>);
}

TEST_CASE("White noise variance matches its power spectral density", "[random][noise]")
{
  // 1e-6 (m/s²)²/Hz sampled at 100 Hz
  constexpr auto psd = 1e-6;
  constexpr auto period = 10ms;
  auto const noise = WhiteNoise::from_psd(psd, period);
  CHECK_THAT(noise.sigma, WithinRel(0.01, 1e-15));
  CHECK(white_noise_sigma(psd, 0.01s) == noise.sigma);

  auto stream = RandomStream::derive(world_seed, "vt30-1", "test.white");
  auto samples = std::vector<double>(100'000);
  for(auto& sample : samples)
    sample = noise.sample(stream);
  // the standard error of the variance is σ² √(2/n), 0.45 % here
  CHECK_THAT(autocovariance(samples, 0), WithinRel(psd / 0.01, 0.02));
  CHECK_THAT(mean_of(samples), WithinAbs(0.0, 4.0 * noise.sigma / std::sqrt(100'000.0)));
  // independent from sample to sample
  CHECK_THAT(autocovariance(samples, 1) / autocovariance(samples, 0), WithinAbs(0.0, 0.015));
}

TEST_CASE("Gauss–Markov autocorrelation matches its correlation time", "[random][noise]")
{
  constexpr auto sigma = 2.0;
  constexpr auto tau_s = 0.5;
  constexpr auto period_s = 0.01;
  auto stream = RandomStream::derive(world_seed, "vt30-1", "test.gauss_markov");
  auto process = GaussMarkov::with(sigma, 500ms, 10ms, stream.normal(0.0, sigma));
  CHECK_THAT(process.phi, WithinRel(std::exp(-period_s / tau_s), 1e-15));
  CHECK_THAT(process.drive, WithinRel(sigma * std::sqrt(1.0 - process.phi * process.phi), 1e-15));

  auto samples = std::vector<double>(1'000'000);
  for(auto& sample : samples)
    sample = process.advance(stream);
  // about n (1 − φ) / (1 + φ) = 10 000 independent samples: the variance has a standard error
  // of 1.4 %, the correlation coefficients of about 0.01
  auto const variance = autocovariance(samples, 0);
  CHECK_THAT(variance, WithinRel(sigma * sigma, 0.05));
  for(auto const lag : {std::size_t {1}, std::size_t {10}, std::size_t {50}, std::size_t {200}}) {
    INFO("lag " << lag);
    CHECK_THAT(
      autocovariance(samples, lag) / variance,
      WithinAbs(std::exp(-static_cast<double>(lag) * period_s / tau_s), 0.04)
    );
  }
}

TEST_CASE("Random walk variance grows by its density squared per second", "[random][noise]")
{
  constexpr auto density = 0.3;
  constexpr auto period_s = 0.0025;
  auto stream = RandomStream::derive(world_seed, "vt30-1", "test.random_walk");
  auto walk = RandomWalk::with(density, 2.5ms, 1.0);
  CHECK_THAT(walk.step_sigma, WithinRel(density * std::sqrt(period_s), 1e-15));
  CHECK(walk.value == 1.0);

  auto increments = std::vector<double>(100'000);
  for(auto& increment : increments) {
    auto const before = walk.value;
    increment = walk.advance(stream) - before;
  }
  CHECK_THAT(autocovariance(increments, 0), WithinRel(density * density * period_s, 0.02));
  CHECK_THAT(autocovariance(increments, 1) / autocovariance(increments, 0), WithinAbs(0.0, 0.015));
}

TEST_CASE("Every sample draws once whatever the parameters", "[random][noise]")
{
  auto silent_stream = RandomStream::derive(world_seed, "vt30-1", "test.draws");
  auto loud_stream = silent_stream;
  auto silent = GaussMarkov::with(0.0, 1s, 10ms);
  auto loud = GaussMarkov::with(5.0, 200ms, 10ms);
  for(auto sample = 0; sample < 10; ++sample) {
    CHECK(WhiteNoise {.sigma = 0.0}.sample(silent_stream) == 0.0);
    (void)silent.advance(silent_stream);
    (void)WhiteNoise {.sigma = 3.0}.sample(loud_stream);
    (void)loud.advance(loud_stream);
  }
  CHECK(silent.value == 0.0);
  CHECK(silent_stream == loud_stream);

  auto bias_stream = RandomStream::derive(world_seed, "vt30-1", "test.bias");
  auto fork = bias_stream;
  CHECK(turn_on_bias(0.5, 0.1, bias_stream) == 0.5 + 0.1 * fork.normal());
}

TEST_CASE("Invalid noise parameters are rejected", "[random][noise]")
{
  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();

  CHECK_THROWS_AS(white_noise_sigma(-1.0, 10ms), InvalidArgumentError);
  CHECK_THROWS_AS(white_noise_sigma(nan, 10ms), InvalidArgumentError);
  CHECK_THROWS_AS(white_noise_sigma(1.0, 0s), InvalidArgumentError);
  CHECK_THROWS_AS(GaussMarkov::with(-1.0, 1s, 10ms), InvalidArgumentError);
  CHECK_THROWS_AS(GaussMarkov::with(1.0, 0s, 10ms), InvalidArgumentError);
  CHECK_THROWS_AS(GaussMarkov::with(1.0, 1s, 10ms, nan), InvalidArgumentError);
  CHECK_THROWS_AS(RandomWalk::with(-0.1, 10ms), InvalidArgumentError);
  CHECK_THROWS_AS(RandomWalk::with(0.1, -10ms), InvalidArgumentError);
  CHECK_THROWS_AS(
    RandomWalk::with(0.1, std::chrono::duration<double>(std::numeric_limits<double>::infinity())),
    InvalidArgumentError
  );
}
