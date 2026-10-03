#pragma once

#include <cmath>
#include <limits>
#include <numbers>
#include <type_traits>
#include <utility>

#include <radar/mms/common/concepts.h>
#include <radar/mms/common/detail/namespace.h>

/**
 * @defgroup core_math Math
 * @ingroup core
 * @brief Constexpr numeric helpers and small geometry-friendly operations.
 */

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Rounds `val` down to the nearest integer.
 * @details This is a constexpr version of std::floor.
 * @tparam T Number type. Must be a floating point type.
 * @param val Number to round.
 * @ingroup core_math
 */
template <std::floating_point T>
[[nodiscard]] constexpr T floor(T val)
{
  return std::floor(val);
}

/**
 * @brief Returns the absolute value of a number.
 * @details This function simply negates the input if it is negative, and
 * returns the input unchanged if it is non-negative. It is a constexpr
 * version of std::abs.
 * @tparam T Type of the value.
 * @param[in] v Value to take the absolute value of.
 * @return The absolute value of v.
 * @ingroup core_math
 */
template <typename T>
constexpr T abs(T const& v)
{
  return v < T(0) ? -v : v;
}

/**
 * @brief Returns the remainder of the division of x by y.
 * @details Calculates `x - y * floor(x / y)`. For a positive divisor, the
 * result is the least nonnegative floating-point remainder.
 * @tparam T Type of the values.
 * @param[in] x First value.
 * @param[in] y Second value.
 * @return The remainder of the division of x by y.
 * @ingroup core_math
 * @see rem_euclid
 */
template <std::floating_point T>
[[nodiscard]] constexpr T fmod(T const x, T const y)
{
  return x - y * floor(x / y);
}

/**
 * @brief Converts degrees to radians.
 * @details Integral inputs produce an integral result, so any fractional part
 * is discarded by the final conversion.
 * @tparam T Underlying type. Must be an integral or floating-point type.
 * @param degrees Degree value.
 * @return Radian value.
 * @ingroup core_math
 * @see to_degrees
 */
template <Number T>
[[nodiscard]] constexpr T to_radians(T degrees) noexcept
{
  using ComputeT = std::conditional_t<std::floating_point<T>, T, double>;
  return static_cast<T>(
    static_cast<ComputeT>(degrees) * std::numbers::pi_v<ComputeT> / ComputeT {180}
  );
}

/**
 * @brief Converts radians to degrees.
 * @details Integral inputs produce an integral result, so any fractional part
 * is discarded by the final conversion.
 * @tparam T Underlying type. Must be an integral or floating-point type.
 * @param radians Radian value.
 * @return Degree value.
 * @ingroup core_math
 * @see to_radians
 */
template <Number T>
[[nodiscard]] constexpr T to_degrees(T radians) noexcept
{
  using ComputeT = std::conditional_t<std::floating_point<T>, T, double>;
  return static_cast<T>(
    static_cast<ComputeT>(radians) * ComputeT {180} / std::numbers::pi_v<ComputeT>
  );
}

/**
 * @brief Returns true if numbers are approximately equal within given epsilon.
 * @details Compares floating point values using formula `|a - b| <= epsilon`.
 * @tparam T Number type
 * @param a First number
 * @param b Second number
 * @param epsilon Epsilon factor for comparison. Default is
 * std::numeric_limits<T>::epsilon()
 * @return True if numbers are approximately equal within given epsilon.
 * @ingroup core_math
 * @see is_null
 */
template <typename T>
[[nodiscard]] constexpr bool approx_eq(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
{
  if constexpr(std::is_floating_point_v<T>)
    return abs(a - b) <= epsilon;
  else
    return a == b;
}

/**
 * @brief Returns true if the number is approximately zero within a given epsilon.
 * @details Compares floating point values using formula `|a| <= epsilon`.
 * @tparam T Number type
 * @param a Number
 * @param epsilon Epsilon factor for comparison. Default is std::numeric_limits<T>::epsilon()
 * @return True if the number is zero within a given epsilon.
 * @ingroup core_math
 * @see approx_eq
 */
template <typename T>
[[nodiscard]] constexpr bool is_null(T a, T epsilon = std::numeric_limits<T>::epsilon())
{
  return approx_eq(a, static_cast<T>(0), epsilon);
}

/**
 * @brief Calculates Euclidean division, the matching method for rem_euclid().
 * @details Returns the quotient that pairs with `rem_euclid(a, b)` so that
 * `a == div_euclid(a, b) * b + rem_euclid(a, b)` for nonzero `b`.
 * @param a Dividend
 * @param b Divisor
 * @return Euclidean quotient.
 * @pre `b` is nonzero and the quotient is representable by `T`.
 * @ingroup core_math
 * @see rem_euclid
 */
template <Number T>
[[nodiscard]] T div_euclid(T a, T b)
{
  if constexpr(std::is_floating_point_v<T>) {
    auto const q = std::trunc(a / b);
    if(std::fmod(a, b) < 0.0)
      return b > 0.0 ? q - 1.0 : q + 1.0;
    return q;
  }
  else {
    auto const q = a / b;
    if(a % b < 0)
      return b > 0 ? q - 1 : q + 1;
    return q;
  }
}

/**
 * @brief Calculates the least nonnegative remainder of `self (mod rhs)`.
 * @details The returned value is in `[0, abs(b))` for nonzero `b`.
 * @param a Dividend
 * @param b Divisor
 * @return Euclidean remainder.
 * @pre `b` is nonzero and the quotient is representable by `T`.
 * @ingroup core_math
 * @see div_euclid
 */
template <Number T>
[[nodiscard]] T rem_euclid(T a, T b)
{
  if constexpr(std::is_floating_point_v<T>) {
    auto const r = std::fmod(a, b);
    return r < 0.0 ? r + std::abs(b) : r;
  }
  else {
    auto const r = a % b;
    if(r >= 0)
      return r;
    return b > 0 ? r + b : r - b;
  }
}

/**
 * @brief Restricts a value to a closed range.
 * @details If `min` is greater than `max`, the bounds are swapped before the
 * value is clamped.
 * @tparam T Underlying type. Must be an integral or floating-point type.
 * @param value Value to clamp.
 * @param min Lower bound.
 * @param max Upper bound.
 * @return `value` constrained to `[min, max]`.
 * @ingroup core_math
 * @see normalize
 */
template <Number T>
[[nodiscard]] constexpr T clamp(T value, T min, T max)
{
  if(min > max)
    std::swap(min, max);
  if(value < min)
    return min;
  if(value > max)
    return max;
  return value;
}

/**
 * @brief Linearly interpolates between two values.
 * @details Floating-point values are interpolated in the same type. Integral
 * values are computed through double precision and converted back to T, so any
 * fractional part is discarded.
 * @tparam T Underlying type. Must be an integral or floating-point type.
 * @param a Start value.
 * @param b End value.
 * @param t Interpolation factor.
 * @return Interpolated value.
 * @ingroup core_math
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto halfway = lerp(10.0, 20.0, 0.5); // 15.0
 * @endcode
 */
template <Number T>
[[nodiscard]] constexpr T lerp(T a, T b, T t)
{
  if constexpr(std::floating_point<T>)
    return a + (b - a) * t;
  else
    return static_cast<T>(a + (static_cast<double>(b) - a) * t);
}

/**
 * @brief Normalizes a value to the `[0, 1]` range.
 * @details The input is clamped to the `[min, max]` range before normalization.
 * If `min` is greater than `max`, clamping treats the bounds as swapped while
 * the normalization direction remains defined by the original arguments. When
 * both bounds are equal, floating-point overloads return NaN and integral
 * overloads return zero. Integral overloads use integral division.
 * @tparam T Underlying type. Must be an integral or floating-point type.
 * @param value Value to normalize.
 * @param min Lower bound.
 * @param max Upper bound.
 * @return Normalized value.
 * @ingroup core_math
 * @see clamp
 * @see lerp
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto progress = normalize(current, min_value, max_value);
 * auto opacity = lerp(0.25, 1.0, progress);
 * @endcode
 */
template <Number T>
[[nodiscard]] constexpr T normalize(T value, T min, T max)
{
  if(min == max) {
    if constexpr(std::floating_point<T>)
      return std::numeric_limits<T>::quiet_NaN();
    else
      return T {0};
  }

  value = clamp(value, min, max);

  return (value - min) / (max - min);
}

RADAR_COMMON_NAMESPACE_END
