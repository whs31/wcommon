#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>
#include <type_traits>
#include <utility>

#include <whs31/common/concepts.h>
#include <whs31/common/detail/namespace.h>

/**
 * @defgroup core_math Math
 * @ingroup core
 * @brief Constexpr numeric helpers and small geometry-friendly operations.
 */

WHS31_COMMON_NAMESPACE_BEGIN

// NOLINTBEGIN(*-identifier-naming)

/**
 * @brief Quiet not-a-number (NaN) constant.
 * @details Represents an undefined or unrepresentable floating-point result.
 * Comparisons with NaN are unordered, including equality with itself.
 * @ingroup core_math
 */
inline auto constexpr NaN = std::numeric_limits<double>::quiet_NaN();

/**
 * @brief Positive infinity constant.
 * @details Represents positive infinity in IEEE 754 floating-point arithmetic.
 * Useful for representing unbounded values and initializing extrema.
 * @ingroup core_math
 */
inline auto constexpr Inf = std::numeric_limits<double>::infinity();

// NOLINTEND(*-identifier-naming)

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
  if consteval {
    // values of at least 2^(digits - 1) have no fractional bits; zeros, infinities and NaNs map to
    // themselves
    auto constexpr integral_bound = T(1) / std::numeric_limits<T>::epsilon();
    if(val == T(0) or not(val > -integral_bound and val < integral_bound))
      return val;
    auto const truncated = static_cast<T>(static_cast<std::int64_t>(val));
    return truncated > val ? truncated - T(1) : truncated;
  }
  else {
    return std::floor(val);
  }
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
 * @brief Returns true if quantities are approximately equal within a given epsilon.
 * @details Compares the SI magnitudes using formula `|a - b| <= epsilon`. The default epsilon is
 * `std::numeric_limits<double>::epsilon()` in the SI unit.
 * @tparam T Quantity type.
 * @param a First quantity.
 * @param b Second quantity.
 * @param epsilon Largest difference treated as equal.
 * @ingroup core_math
 * @see Quantity
 */
template <Quantity T>
[[nodiscard]] constexpr bool approx_eq(
  T a,
  T b,
  T epsilon = QuantityTraits<T>::from_si(std::numeric_limits<double>::epsilon())
)
{
  return approx_eq(static_cast<double>(a), static_cast<double>(b), static_cast<double>(epsilon));
}

/**
 * @brief Returns true if the quantity is approximately zero within a given epsilon.
 * @tparam T Quantity type.
 * @param a Quantity.
 * @param epsilon Largest magnitude treated as zero.
 * @ingroup core_math
 * @see Quantity
 */
template <Quantity T>
[[nodiscard]] constexpr bool is_null(
  T a,
  T epsilon = QuantityTraits<T>::from_si(std::numeric_limits<double>::epsilon())
)
{
  return is_null(static_cast<double>(a), static_cast<double>(epsilon));
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
 * using namespace whs31;
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
 * @brief Restricts a quantity to a closed range.
 * @details If `min` is greater than `max`, the bounds are swapped before the quantity is clamped.
 * @tparam T Quantity type.
 * @param value Quantity to clamp.
 * @param min Lower bound.
 * @param max Upper bound.
 * @return `value` constrained to `[min, max]`.
 * @ingroup core_math
 * @see Quantity
 */
template <Quantity T>
[[nodiscard]] constexpr T clamp(T value, std::type_identity_t<T> min, std::type_identity_t<T> max)
{
  return QuantityTraits<T>::from_si(
    clamp(static_cast<double>(value), static_cast<double>(min), static_cast<double>(max))
  );
}

/**
 * @brief Linearly interpolates between two quantities.
 * @details For an Angle the interpolation is linear in radians. Use Angle::lerp() to interpolate
 * along the shortest arc instead.
 * @tparam T Quantity type.
 * @param a Start quantity.
 * @param b End quantity.
 * @param t Interpolation factor.
 * @return Interpolated quantity.
 * @ingroup core_math
 * @see Quantity
 *
 * @code {.cpp}
 * using namespace whs31;
 *
 * auto halfway = lerp(10_m, 20_m, 0.5); // 15 m
 * @endcode
 */
template <Quantity T>
[[nodiscard]] constexpr T lerp(T a, std::type_identity_t<T> b, double const t)
{
  return QuantityTraits<T>::from_si(lerp(static_cast<double>(a), static_cast<double>(b), t));
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
 * using namespace whs31;
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
      return NaN;
    else
      return T {0};
  }

  value = clamp(value, min, max);

  return (value - min) / (max - min);
}

/**
 * @brief Checks whether a floating-point value is neither infinite nor NaN.
 * @details Unlike `std::isfinite`, it is usable in constant expressions on every compiler.
 * @note Inside a class with an `is_finite()` member, e.g. Vector3, the member hides this
 * function; call it as `common::is_finite(value)` there.
 * @tparam T Floating-point type.
 * @param value Value to check.
 * @ingroup core_math
 * @see positive
 * @see nonnegative
 * @see in_closed_range
 */
template <std::floating_point T>
[[nodiscard]] constexpr bool is_finite(T const value) noexcept
{
  // NaN fails every comparison, and an infinity exceeds the largest finite value
  return abs(value) <= std::numeric_limits<T>::max();
}

/**
 * @brief Checks whether a quantity has a finite SI magnitude.
 * @ingroup core_math
 * @see Quantity
 */
template <Quantity T>
[[nodiscard]] constexpr bool is_finite(T const value) noexcept
{
  return is_finite(static_cast<double>(value));
}

/**
 * @brief Checks whether a value is finite and greater than zero.
 * @details A NaN never passes. This avoids the common pitfall of writing `not (x <= 0)`, which
 * is `true` for NaN.
 * @tparam T Floating-point type.
 * @param value Value to check.
 * @ingroup core_math
 * @see nonnegative
 */
template <std::floating_point T>
[[nodiscard]] constexpr bool positive(T const value) noexcept
{
  return is_finite(value) and value > T {0};
}

/**
 * @brief Checks whether a quantity is finite and greater than zero.
 * @ingroup core_math
 * @see Quantity
 */
template <Quantity T>
[[nodiscard]] constexpr bool positive(T const value) noexcept
{
  return positive(static_cast<double>(value));
}

/**
 * @brief Checks whether a value is finite and not less than zero.
 * @tparam T Floating-point type.
 * @param value Value to check.
 * @ingroup core_math
 * @see positive
 */
template <std::floating_point T>
[[nodiscard]] constexpr bool nonnegative(T const value) noexcept
{
  return is_finite(value) and value >= T {0};
}

/**
 * @brief Checks whether a quantity is finite and not less than zero.
 * @ingroup core_math
 * @see Quantity
 */
template <Quantity T>
[[nodiscard]] constexpr bool nonnegative(T const value) noexcept
{
  return nonnegative(static_cast<double>(value));
}

/**
 * @brief Checks whether a value is finite and inside the closed range `[lo, hi]`.
 * @details The bounds take the type of the checked value, so bounds of another arithmetic type
 * need no casts, e.g. `in_closed_range(float_soc, 0.0, 1.0)`.
 * @tparam T Floating-point type.
 * @param value Value to check.
 * @param lo Lower bound.
 * @param hi Upper bound.
 * @ingroup core_math
 * @see clamp
 */
template <std::floating_point T>
[[nodiscard]] constexpr bool in_closed_range(
  T const value,
  std::type_identity_t<T> const lo,
  std::type_identity_t<T> const hi
) noexcept
{
  return is_finite(value) and value >= lo and value <= hi;
}

/**
 * @brief Checks whether a quantity is finite and inside the closed range `[lo, hi]`.
 * @details For example, `in_closed_range(pitch, -90_deg, 90_deg)`.
 * @ingroup core_math
 * @see Quantity
 */
template <Quantity T>
[[nodiscard]] constexpr bool in_closed_range(
  T const value,
  std::type_identity_t<T> const lo,
  std::type_identity_t<T> const hi
) noexcept
{
  return in_closed_range(
    static_cast<double>(value),
    static_cast<double>(lo),
    static_cast<double>(hi)
  );
}

/**
 * @brief Rounds a value to the nearest multiple of `step`.
 * @details Halfway cases round away from zero. A `step` that is not positive leaves the value
 * unchanged.
 * @tparam T Floating-point type.
 * @param value Value to round.
 * @param step Quantization step.
 * @ingroup core_math
 */
template <std::floating_point T>
[[nodiscard]] constexpr T quantized(T const value, std::type_identity_t<T> const step) noexcept
{
  return step > T {0} ? std::round(value / step) * step : value;
}

WHS31_COMMON_NAMESPACE_END
