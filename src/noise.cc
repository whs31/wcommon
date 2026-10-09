#include <radar/mms/common/noise.h>

#include <cmath>

#include <radar/mms/common/check.h>
#include <radar/mms/common/error.h>
#include <radar/mms/common/math.h>

// see random.cc: the samples must round identically on every platform
#ifdef __FAST_MATH__
#  error "deterministic noise processes cannot be built with -ffast-math"
#endif

RADAR_COMMON_NAMESPACE_BEGIN

double white_noise_sigma(double const psd, std::chrono::duration<double> const sample_period)
{
  RADAR_HARD_CHECK(  // NOLINT(*-avoid-do-while)
    nonnegative(psd) and positive(sample_period.count()),
    InvalidArgumentError,
    "white noise needs a non-negative density and a positive sample period, got {} and {} s",
    psd,
    sample_period.count()
  );
  return std::sqrt(psd / sample_period.count());
}

WhiteNoise WhiteNoise::from_psd(double const psd, std::chrono::duration<double> const sample_period)
{
  return {.sigma = white_noise_sigma(psd, sample_period)};
}

GaussMarkov GaussMarkov::with(
  double const sigma,
  std::chrono::duration<double> const correlation_time,
  std::chrono::duration<double> const sample_period,
  double const initial
)
{
  RADAR_HARD_CHECK(  // NOLINT(*-avoid-do-while)
    nonnegative(sigma) and positive(correlation_time.count()) and positive(sample_period.count())
      and is_finite(initial),
    InvalidArgumentError,
    "Gauss–Markov process needs a non-negative deviation, a positive correlation time and "
    "period and a finite start, got {}, {} s, {} s and {}",
    sigma,
    correlation_time.count(),
    sample_period.count(),
    initial
  );
  auto const phi = portable_exp(-sample_period.count() / correlation_time.count());
  return {.phi = phi, .drive = sigma * std::sqrt(1.0 - phi * phi), .value = initial};
}

double GaussMarkov::advance(RandomStream& stream) noexcept
{
  this->value = this->phi * this->value + this->drive * stream.normal();
  return this->value;
}

RandomWalk RandomWalk::with(
  double const density,
  std::chrono::duration<double> const sample_period,
  double const initial
)
{
  RADAR_HARD_CHECK(  // NOLINT(*-avoid-do-while)
    nonnegative(density) and positive(sample_period.count()) and is_finite(initial),
    InvalidArgumentError,
    "random walk needs a non-negative density, a positive period and a finite start, got {}, "
    "{} s and {}",
    density,
    sample_period.count(),
    initial
  );
  return {.step_sigma = density * std::sqrt(sample_period.count()), .value = initial};
}

double RandomWalk::advance(RandomStream& stream) noexcept
{
  this->value += this->step_sigma * stream.normal();
  return this->value;
}

double turn_on_bias(double const fixed, double const sigma, RandomStream& stream) noexcept
{
  return fixed + sigma * stream.normal();
}

RADAR_COMMON_NAMESPACE_END
