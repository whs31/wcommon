#pragma once

#include <cstdint>
#include <array>
#include <bit>
#include <string_view>

#include <radar/mms/common/check.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/error.h>

/**
 * @defgroup core_random Random
 * @ingroup core
 * @brief Deterministic random streams: the same seed gives bit-identical draws on every platform.
 * @details `<random>` distributions are implementation-defined, and `std::log`, `std::sin` and
 * `std::cos` may differ by an ulp between C runtimes, so seeded results differ between Linux,
 * MinGW and MSVC builds. The samplers here are built only from operations IEEE 754 rounds exactly
 * (`+`, `-`, `*`, `/`, `sqrt`, `frexp`, `ldexp`), so they replay identically everywhere.
 *
 * See the "Random streams" documentation page for the seeding scheme and the build requirements.
 * @see RandomStream
 */

RADAR_COMMON_NAMESPACE_BEGIN

namespace _random_impl // NOLINT(*-identifier-naming)
{
  /**
   * @brief One step of SplitMix64: advances `state` and returns its next output.
   * @details Seeds the generator state, so that seeds differing in a single bit give unrelated
   * streams.
   */
  [[nodiscard]] constexpr std::uint64_t splitmix64(std::uint64_t& state) noexcept
  {
    state += 0x9e37'79b9'7f4a'7c15U;
    auto z = state;
    z = (z ^ (z >> 30U)) * 0xbf58'476d'1ce4'e5b9U;
    z = (z ^ (z >> 27U)) * 0x94d0'49bb'1331'11ebU;
    return z ^ (z >> 31U);
  }

  // splitmix64 is a bijection of its counter, so four consecutive outputs are never all zero,
  // the one state xoshiro256** cannot leave
  [[nodiscard]] constexpr std::array<std::uint64_t, 4> seeded_state(std::uint64_t seed) noexcept
  {
    auto state = std::array<std::uint64_t, 4> {};
    for(auto& word : state)
      word = splitmix64(seed);
    return state;
  }

  // 64-bit FNV-1a; values are fed little-endian on every platform, so seeds do not depend on the
  // byte order
  class Fnv1a
  {
   public:
    constexpr void feed(std::uint64_t const value) noexcept
    {
      for(auto shift = 0U; shift < 64U; shift += 8U)
        this->feed_byte(static_cast<std::uint8_t>(value >> shift));
    }

    // length-prefixed, so ("ab", "c") and ("a", "bc") hash differently
    constexpr void feed(std::string_view const text) noexcept
    {
      this->feed(static_cast<std::uint64_t>(text.size()));
      for(auto const c : text)
        this->feed_byte(static_cast<std::uint8_t>(c));
    }

    [[nodiscard]] constexpr std::uint64_t value() const noexcept { return this->m_hash; }

   private:
    constexpr void feed_byte(std::uint8_t const byte) noexcept
    {
      this->m_hash = (this->m_hash ^ byte) * 0x0000'0100'0000'01b3U;
    }

    std::uint64_t m_hash = 0xcbf2'9ce4'8422'2325U;
  };
}  // namespace _random_impl

/**
 * @brief Natural logarithm of a positive finite @p x, bit-identical on every platform.
 * @details The fdlibm/musl algorithm: `x = m · 2^k` with `m` in `[√½, √2)`, and `log(m)` from a
 * minimax polynomial in `s = (m − 1) / (m + 1)`. Within 1 ulp of the correctly rounded result.
 * Use it instead of `std::log` wherever a result must replay identically across platforms.
 * @pre @p x is positive and finite.
 * @ingroup core_random
 * @see portable_exp
 */
[[nodiscard]] RADAR_COMMON_API double portable_log(double x) noexcept;

/**
 * @brief Exponential of a finite @p x in `[-708, 709]`, bit-identical on every platform.
 * @details The fdlibm/musl algorithm: `x = k ln 2 + r` with `|r| ≤ ½ ln 2`, `exp(r)` from a
 * minimax rational approximation, scaled by `2^k` with the exact `ldexp`. Within 1 ulp of the
 * correctly rounded result.
 * @pre @p x is finite and in `[-708, 709]`.
 * @ingroup core_random
 * @see portable_log
 */
[[nodiscard]] RADAR_COMMON_API double portable_exp(double x) noexcept;

/**
 * @brief Returns the seed of the stream @p stream owned by @p scope, e.g.
 * `stream_seed(world_seed, "vt30-1", "imu.gyro")`.
 * @details A length-prefixed 64-bit FNV-1a hash of the three values. It depends on them only,
 * never on object identities or creation order, so adding another owner or stream leaves every
 * existing stream unchanged.
 * @param seed Top-level seed, e.g. of a simulation run.
 * @param scope Stable name of the owner, e.g. a vehicle name.
 * @param stream Stable name of the stream within its owner, e.g. `"imu.gyro"`.
 * @ingroup core_random
 * @see RandomStream::derive
 */
[[nodiscard]] constexpr std::uint64_t stream_seed(
  std::uint64_t const seed,
  std::string_view const scope,
  std::string_view const stream
) noexcept
{
  auto hash = _random_impl::Fnv1a();
  hash.feed(seed);
  hash.feed(scope);
  hash.feed(stream);
  return hash.value();
}

/**
 * @brief A reproducible random stream: the xoshiro256** generator and its samplers.
 * @details The whole state is 32 bytes in a trivially copyable aggregate, so a stream lives inside
 * the object that draws from it and is saved and restored with it; avoid keeping one in a global.
 * Copying a stream forks it: both copies give the same draws.
 *
 * Use one stream per independent noise source, so that adding a draw to one source never shifts
 * another.
 *
 * The default stream is `from_seed(0)`. Do not write `state` by hand: the all-zero state is stuck
 * at zero.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto gyro_noise = RandomStream::derive(world_seed, "vt30-1", "imu.gyro");
 * auto const noise_rad_per_s = gyro_noise.normal(0.0, sigma_rad_per_s);
 * @endcode
 *
 * @ingroup core_random
 */
struct RADAR_COMMON_API RandomStream
{
  std::array<std::uint64_t, 4> state = _random_impl::seeded_state(0);

  /**
   * @brief Returns the stream seeded through SplitMix64, e.g. by stream_seed().
   */
  [[nodiscard]] static constexpr RandomStream from_seed(std::uint64_t const seed) noexcept
  {
    return {.state = _random_impl::seeded_state(seed)};
  }

  /**
   * @brief Returns the independent stream `from_seed(stream_seed(seed, scope, stream))`.
   * @see stream_seed
   */
  [[nodiscard]] static constexpr RandomStream derive(
    std::uint64_t const seed,
    std::string_view const scope,
    std::string_view const stream
  ) noexcept
  {
    return from_seed(stream_seed(seed, scope, stream));
  }

  /**
   * @brief Returns the next 64 uniformly distributed bits.
   */
  [[nodiscard]] constexpr std::uint64_t next() noexcept
  {
    auto& [s0, s1, s2, s3] = this->state;
    auto const result = std::rotl(s1 * 5U, 7) * 9U;
    auto const t = s1 << 17U;
    s2 ^= s0;
    s3 ^= s1;
    s1 ^= s2;
    s0 ^= s3;
    s2 ^= t;
    s3 = std::rotl(s3, 45);
    return result;
  }

  /**
   * @brief Returns a uniform value in `[0, 1)`, a multiple of 2⁻⁵³.
   */
  [[nodiscard]] constexpr double uniform() noexcept
  {
    return static_cast<double>(this->next() >> 11U) * 0x1.0p-53;
  }

  /**
   * @brief Returns a uniform value in `[lo, hi]`; rounding may return @p hi.
   */
  [[nodiscard]] double uniform(double lo, double hi) noexcept;

  /**
   * @brief Returns a uniform whole number in `[0, bound)`, without modulo bias.
   * @throws InvalidArgumentError if @p bound is zero.
   */
  [[nodiscard]] constexpr std::uint64_t below(std::uint64_t const bound)
  {
    // NOLINTNEXTLINE(*-avoid-do-while)
    RADAR_HARD_CHECK(bound > 0, InvalidArgumentError, "random bound must be positive");

    // the draws below 2^64 mod bound would make the low remainders more likely
    auto const threshold = (std::uint64_t {0} - bound) % bound;
    while(true) {
      if(auto const bits = this->next(); bits >= threshold)
        return bits % bound;
    }
  }

  /**
   * @brief Returns a standard normal value, by the Marsaglia polar method.
   * @details Unlike Box–Muller it needs no sine or cosine, and its logarithm is portable_log().
   * Each call draws the uniforms of its own pair and returns one value: there is no hidden spare
   * value, so the whole state is `state`.
   */
  [[nodiscard]] double normal() noexcept;

  /**
   * @brief Returns a normal value with the given mean and standard deviation.
   */
  [[nodiscard]] double normal(double mean, double stddev) noexcept;

  [[nodiscard]] constexpr bool operator==(RandomStream const&) const noexcept = default;
};

RADAR_COMMON_NAMESPACE_END
