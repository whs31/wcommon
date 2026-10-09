#pragma once

#include <chrono>

#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/random.h>

/**
 * @file
 * @brief Discrete-time noise processes driven by RandomStream: white noise, first-order
 * Gauss–Markov processes, random walks and turn-on biases.
 * @details The processes are plain aggregates discretized for one sample period, so they live
 * inside the state of their owner next to the RandomStream they draw from. Every sample draws
 * exactly once from its stream, whatever the parameters, so changing one noise source never
 * shifts the draws of another. Only operations IEEE 754 rounds exactly and portable_exp() are
 * used, so the samples are bit-identical on every platform.
 */

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Returns the standard deviation `√(PSD / Tₛ)` of band-limited white noise of the power
 * spectral density @p psd sampled every @p sample_period.
 * @details This is how the Simulink *Band-Limited White Noise* block defines it from its
 * *Noise power*. For example, a density of 1e-6 (m/s²)²/Hz sampled at 100 Hz gives
 * σ = 0.01 m/s².
 * @throws InvalidArgumentError if the density is negative or not finite, or the period is not
 * positive.
 * @ingroup core_random
 * @see WhiteNoise
 */
[[nodiscard]] RADAR_COMMON_API double white_noise_sigma(
  double psd,
  std::chrono::duration<double> sample_period
);

/**
 * @brief Zero-mean white Gaussian noise, independent from sample to sample.
 * @ingroup core_random
 */
struct RADAR_COMMON_API WhiteNoise
{
  double sigma = 0.0;

  /**
   * @brief Returns the noise of the density @p psd sampled every @p sample_period.
   * @throws InvalidArgumentError as white_noise_sigma().
   */
  [[nodiscard]] static WhiteNoise from_psd(double psd, std::chrono::duration<double> sample_period);

  /**
   * @brief Draws one sample.
   */
  [[nodiscard]] double sample(RandomStream& stream) const noexcept
  {
    return this->sigma * stream.normal();
  }
};

/**
 * @brief First-order Gauss–Markov process `ẋ = −x/τ + w`, sampled exactly every `Tₛ`: a slowly
 * wandering bias.
 * @details `x[k+1] = φ x[k] + σ √(1 − φ²) w[k]` with `φ = exp(−Tₛ/τ)` and `w` standard normal, so
 * a stationary process keeps the standard deviation σ and has the autocorrelation `σ² φ^|k|` at
 * a lag of k samples, whatever the period.
 * @ingroup core_random
 */
struct RADAR_COMMON_API GaussMarkov
{
  /// φ = exp(−Tₛ/τ).
  double phi = 0.0;
  /// σ √(1 − φ²).
  double drive = 0.0;
  double value = 0.0;

  /**
   * @brief Returns the process discretized for @p sample_period.
   * @param sigma Stationary standard deviation σ.
   * @param correlation_time Correlation time τ.
   * @param sample_period Sample period Tₛ.
   * @param initial `x[0]`, e.g. `stream.normal(0.0, sigma)` to start stationary.
   * @throws InvalidArgumentError if the deviation is negative, the correlation time or the period
   * is not positive, or a value is not finite.
   */
  [[nodiscard]] static GaussMarkov with(
    double sigma,
    std::chrono::duration<double> correlation_time,
    std::chrono::duration<double> sample_period,
    double initial = 0.0
  );

  /**
   * @brief Advances by one sample period and returns the new value.
   */
  double advance(RandomStream& stream) noexcept;
};

/**
 * @brief Random walk, the integral of white noise of the density `density` (units/√s): a drifting
 * bias, e.g. the angle random walk of a gyro.
 * @details `x[k+1] = x[k] + density √Tₛ w[k]`, so the variance grows by `density² Tₛ` per sample.
 * @ingroup core_random
 */
struct RADAR_COMMON_API RandomWalk
{
  /// density √Tₛ.
  double step_sigma = 0.0;
  double value = 0.0;

  /**
   * @brief Returns the walk discretized for @p sample_period.
   * @throws InvalidArgumentError if the density is negative, the period is not positive, or a
   * value is not finite.
   */
  [[nodiscard]] static RandomWalk with(
    double density,
    std::chrono::duration<double> sample_period,
    double initial = 0.0
  );

  /**
   * @brief Advances by one sample period and returns the new value.
   */
  double advance(RandomStream& stream) noexcept;
};

/**
 * @brief Returns a constant bias: the fixed part @p fixed plus a turn-on part of the standard
 * deviation @p sigma, drawn once, e.g. when a sensor is built.
 * @ingroup core_random
 */
[[nodiscard]] RADAR_COMMON_API double turn_on_bias(
  double fixed,
  double sigma,
  RandomStream& stream
) noexcept;

RADAR_COMMON_NAMESPACE_END
