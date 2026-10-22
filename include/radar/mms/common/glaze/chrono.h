#pragma once

#include <cmath>
#include <cstdint>

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <concepts>
#include <limits>
#include <memory>
#include <optional>
#include <ratio>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>

#include <glaze/chrono.hpp>

#include <radar/mms/common/glaze/detail.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct DurationFields
  {
    std::optional<double> nanoseconds;
    std::optional<double> microseconds;
    std::optional<double> milliseconds;
    std::optional<double> seconds;
    std::optional<double> minutes;
    std::optional<double> hours;
  };

  /**
   * @brief Every unit a duration of type `Duration` may be read in: its key, the suffix of its
   * `std::chrono` literal, and the conversion of a magnitude in it.
   */
  template <typename Duration>
  struct DurationReading
  {
    using rep = Duration::rep;
    using period = Duration::period;
    // the duration in its own ticks, before rounding to its count type
    using exact = std::chrono::duration<double, period>;

    struct Unit
    {
      std::optional<double> DurationFields::* field = nullptr;
      std::string_view suffix;
      void (*assign)(Duration&, double, glz::context&) noexcept = nullptr;
    };

    /**
     * @brief Assigns @p value from @p magnitude in units of @p UnitPeriod, failing the read with
     * `constraint_violated` if an integer `Duration` cannot hold it exactly.
     */
    template <typename UnitPeriod>
    static void assign(Duration& value, double const magnitude, glz::context& ctx) noexcept
    {
      auto const source = std::chrono::duration<double, UnitPeriod>(magnitude);
      auto const ticks = std::chrono::duration_cast<exact>(source).count();
      if constexpr(std::floating_point<rep>) {
        value = Duration(static_cast<rep>(ticks));
      }
      else {
        // 2^digits is the first value above the range, and exact as a double
        auto const above = std::ldexp(1.0, std::numeric_limits<rep>::digits);
        auto const lowest = std::numeric_limits<rep>::is_signed ? -above : 0.0;
        if(ticks != std::trunc(ticks) or ticks < lowest or ticks >= above) {
          reject(ctx, "a duration must be a whole number of the ticks of its type");
          return;
        }
        value = Duration(static_cast<rep>(ticks));
      }
    }

    static constexpr auto units = std::array {
      Unit {&DurationFields::nanoseconds, "ns", &assign<std::nano>},
      Unit {&DurationFields::microseconds, "us", &assign<std::micro>},
      Unit {&DurationFields::milliseconds, "ms", &assign<std::milli>},
      Unit {&DurationFields::seconds, "s", &assign<std::ratio<1>>},
      Unit {&DurationFields::minutes, "min", &assign<std::ratio<60>>},
      Unit {&DurationFields::hours, "h", &assign<std::ratio<3'600>>},
    };

    /// @brief What a duration is read from in a text format.
    using Input = std::variant<rep, DurationFields, std::string>;

    static void read(Duration& value, Input const& input, glz::context& ctx) noexcept
    {
      if(auto const* const count = std::get_if<rep>(&input))
        value = Duration(*count);
      else if(auto const* const fields = std::get_if<DurationFields>(&input))
        read_fields(value, *fields, ctx);
      else if(auto const* const literal = std::get_if<std::string>(&input))
        read_literal(value, *literal, ctx);
    }

    static void read_fields(
      Duration& value,
      DurationFields const& fields,
      glz::context& ctx
    ) noexcept
    {
      auto const present = std::ranges::count_if(units, [&fields](Unit const& unit) {
        return (fields.*unit.field).has_value();
      });
      if(present != 1) {
        reject(ctx, "a duration must hold its magnitude in exactly one unit");
        return;
      }
      for(auto const& unit : units)
        if(auto const& magnitude = fields.*unit.field; magnitude.has_value())
          unit.assign(value, *magnitude, ctx);
    }

    static void read_literal(
      Duration& value,
      std::string_view const literal,
      glz::context& ctx
    ) noexcept
    {
      auto magnitude = 0.0;
      auto const* const last = std::to_address(literal.end());
      auto const [suffix_begin, error] = std::
        from_chars(std::to_address(literal.begin()), last, magnitude);
      if(error != std::errc {}) {
        reject(ctx, "a duration literal must start with its magnitude");
        return;
      }
      auto const
        unit = std::ranges::find(units, std::string_view(suffix_begin, last), &Unit::suffix);
      if(unit == units.end()) {
        reject(ctx, "a duration literal needs one of the unit suffixes ns, us, ms, s, min, h");
        return;
      }
      unit->assign(value, magnitude, ctx);
    }
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of `std::chrono::duration`: replaces glaze's reading of a duration, which
 * takes the count only, by the reading of the glz::from specialization below.
 * @ingroup core_glaze
 */
template <typename Rep, typename Period>
struct glz::meta<std::chrono::duration<Rep, Period>>
{
  static auto constexpr custom_read = true;
};

/**
 * @brief Reads a `std::chrono::duration`: its count, as glaze and the nlohmann adapter in
 * `json/chrono.h` write it, or, in a text format, the object of one unit key or a literal string.
 * @details Writing is glaze's, the count in the ticks of the duration's type, so documents stay
 * readable by the nlohmann adapter. Reading also accepts, as for the quantities:
 *
 * - `{"milliseconds": 7.5}`, with one of the keys `nanoseconds`, `microseconds`, `milliseconds`,
 *   `seconds`, `minutes` and `hours`;
 * - a string spelled like the `std::chrono` literal: `"7.5ms"`, `"2s"`, `"1.5h"`, with the
 *   suffixes `ns`, `us`, `ms`, `s`, `min` and `h`.
 *
 * Both convert to the duration's type; a duration with an integer count fails with
 * `glz::error_code::constraint_violated` if the value is not a whole number of its ticks, e.g.
 * `"1500us"` for `std::chrono::milliseconds`. Binary formats such as BEVE read the count only.
 *
 * @code {.cpp}
 * struct Link
 * {
 *   std::chrono::duration<double, std::milli> latency;
 *   std::chrono::seconds timeout;
 * };
 *
 * // latency = "7.5ms"
 * // timeout = { minutes = 2 }
 * auto link = Link {};
 * auto const error = glz::read_toml(link, document);
 * @endcode
 *
 * @note Include this header wherever durations are read: without it glaze reads the count only.
 * @ingroup core_glaze
 */
template <std::uint32_t Format, typename Rep, typename Period>
struct glz::from<Format, std::chrono::duration<Rep, Period>>
{
  using duration_type = std::chrono::duration<Rep, Period>;
  using reading = radar::mms::common::_glaze_impl::DurationReading<duration_type>;

  template <auto Opts, typename... Args>
  static void op(duration_type& value, is_context auto&& ctx, Args&&... args)
  {
    if constexpr(Format == glz::JSON or Format == glz::TOML) {
      auto input = typename reading::Input {};
      glz::from<Format, typename reading::Input>::template op<
        Opts>(input, ctx, std::forward<Args>(args)...);
      if(not bool(ctx.error))
        reading::read(value, input, ctx);
    }
    else {
      auto count = Rep {};
      glz::from<Format, Rep>::template op<Opts>(count, ctx, std::forward<Args>(args)...);
      if(not bool(ctx.error))
        value = duration_type(count);
    }
  }

  // binary formats that read the type tag of a value before it, e.g. MessagePack
  template <auto Opts, typename... Args>
  static void op(
    duration_type& value,
    std::uint8_t const tag,
    is_context auto&& ctx,
    Args&&... args
  )
  {
    auto count = Rep {};
    glz::from<Format, Rep>::template op<Opts>(count, tag, ctx, std::forward<Args>(args)...);
    if(not bool(ctx.error))
      value = duration_type(count);
  }
};
