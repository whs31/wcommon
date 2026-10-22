#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <iterator>
#include <span>

#include <radar/mms/common/check.h>
#include <radar/mms/common/concepts.h>
#include <radar/mms/common/error.h>
#include <radar/mms/common/math.h>
#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Concept of a Curve axis type: a floating-point number or a Quantity.
 * @ingroup core_math
 * @see Curve
 */
template <typename T>
concept CurveAxis = std::floating_point<T> or Quantity<T>;

namespace _curve_impl  // NOLINT(*-identifier-naming)
{
  // the magnitude a curve computes with: the value itself, or the SI magnitude of a quantity
  template <CurveAxis T>
  [[nodiscard]] constexpr auto curve_magnitude(T const value) noexcept
  {
    if constexpr(std::floating_point<T>)
      return value;
    else
      return static_cast<double>(value);
  }
}  // namespace _curve_impl

/**
 * @brief Piecewise-linear lookup table `value = f(argument)` holding up to `MaxPoints` points.
 * @details Points are stored inline, so the curve never allocates and is trivially copyable for
 * trivially copyable axis types. It suits motor, battery and other 1-D calibration tables
 * embedded in simulation state.
 *
 * A non-empty curve always has finite points with strictly increasing arguments; this is checked
 * once on construction. Interpolation clamps to the end values outside the argument range.
 *
 * Both axes may be floating-point numbers of any precision or quantities, e.g.
 * `Curve<Angle, double, 8>`; quantities are interpolated by their SI magnitudes.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto const open_circuit_voltage = Curve<double, double, 4>(
 *   std::array {0.0, 0.2, 0.8, 1.0},     // state of charge
 *   std::array {19.8, 21.6, 24.0, 25.2}  // volts
 * );
 * auto const voltage = open_circuit_voltage.interpolate(0.5);  // 22.8
 * @endcode
 *
 * @tparam X Argument type; may differ in precision from `Y`.
 * @tparam Y Value type.
 * @tparam MaxPoints Maximum number of points the curve can hold.
 * @ingroup core_math
 */
template <CurveAxis X, CurveAxis Y, std::size_t MaxPoints>
  requires(MaxPoints > 0)
class Curve
{
 public:
  using argument_type = X;
  using value_type = Y;

  static constexpr std::size_t max_points = MaxPoints;

  /**
   * @brief Creates an empty curve, which cannot be interpolated.
   */
  constexpr Curve() noexcept = default;

  /**
   * @brief Creates a curve from its arguments and the values at them.
   * @throws InvalidArgumentError if the point count is outside `[1, MaxPoints]`, the spans differ
   * in size, a point is not finite, or the arguments are not strictly increasing.
   */
  constexpr Curve(std::span<X const> const arguments, std::span<Y const> const values)
    : m_size(arguments.size())
  {
    RADAR_HARD_CHECK(  // NOLINT(*-avoid-do-while)
      arguments.size() == values.size(),
      InvalidArgumentError,
      "curve has {} arguments but {} values",
      arguments.size(),
      values.size()
    );
    RADAR_HARD_CHECK(  // NOLINT(*-avoid-do-while)
      not arguments.empty() and arguments.size() <= MaxPoints,
      InvalidArgumentError,
      "curve must have 1..{} points, got {}",
      MaxPoints,
      arguments.size()
    );
    RADAR_HARD_CHECK(  // NOLINT(*-avoid-do-while)
      std::ranges::all_of(arguments, [](X const x) { return is_finite(x); })
        and std::ranges::all_of(values, [](Y const y) { return is_finite(y); })
        and std::ranges::adjacent_find(
              arguments,
              std::ranges::greater_equal {},
              _curve_impl::curve_magnitude<X>
            ) == arguments.end(),
      InvalidArgumentError,
      "curve points must be finite with strictly increasing arguments"
    );
    std::ranges::copy(arguments, this->m_arguments.begin());
    std::ranges::copy(values, this->m_values.begin());
  }

  [[nodiscard]] constexpr std::span<X const> arguments() const noexcept
  {
    return {this->m_arguments.data(), this->m_size};
  }

  [[nodiscard]] constexpr std::span<Y const> values() const noexcept
  {
    return {this->m_values.data(), this->m_size};
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept { return this->m_size; }

  [[nodiscard]] constexpr bool empty() const noexcept { return this->m_size == 0; }

  /**
   * @brief Interpolates linearly between the neighbouring points, clamped to the end values
   * outside the argument range.
   * @throws InvalidArgumentError if the curve is empty or @p argument is not finite.
   */
  [[nodiscard]] constexpr Y interpolate(X const argument) const
  {
    using _curve_impl::curve_magnitude;

    // NOLINTNEXTLINE(*-avoid-do-while)
    RADAR_HARD_CHECK(not this->empty(), InvalidArgumentError, "cannot interpolate an empty curve");
    RADAR_HARD_CHECK(  // NOLINT(*-avoid-do-while)
      is_finite(argument),
      InvalidArgumentError,
      "cannot interpolate a curve at {}",
      curve_magnitude(argument)
    );
    auto const x = this->arguments();
    auto const y = this->values();
    auto const at = curve_magnitude(argument);
    if(at <= curve_magnitude(x.front()))
      return y.front();
    if(at >= curve_magnitude(x.back()))
      return y.back();

    // the end checks above keep the upper bound strictly inside the curve, so both neighbours exist
    auto const x1 = std::ranges::upper_bound(x, at, {}, curve_magnitude<X>);
    auto const y1 = std::next(y.begin(), std::distance(x.begin(), x1));
    auto const x0 = curve_magnitude(*std::prev(x1));
    auto const fraction = (at - x0) / (curve_magnitude(*x1) - x0);
    if constexpr(std::floating_point<Y>) {
      return *std::prev(y1) + static_cast<Y>(fraction) * (*y1 - *std::prev(y1));
    }
    else {
      auto const y0 = curve_magnitude(*std::prev(y1));
      return QuantityTraits<Y>::from_si(
        y0 + static_cast<double>(fraction) * (curve_magnitude(*y1) - y0)
      );
    }
  }

  /**
   * @brief Compares the points of two curves.
   */
  [[nodiscard]] constexpr bool operator==(Curve const& other) const noexcept
  {
    return std::ranges::equal(this->arguments(), other.arguments())
       and std::ranges::equal(this->values(), other.values());
  }

 private:
  std::array<X, MaxPoints> m_arguments {};
  std::array<Y, MaxPoints> m_values {};
  std::size_t m_size = 0;
};

RADAR_COMMON_NAMESPACE_END
