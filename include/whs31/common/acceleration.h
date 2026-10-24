#pragma once

#include <chrono>
#include <string_view>

#include <whs31/common/config.h>
#include <whs31/common/constants.h>
#include <whs31/common/detail/export.h>
#include <whs31/common/detail/namespace.h>
#include <whs31/common/quantity.h>
#include <whs31/common/velocity.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Acceleration value stored in `m/s^2`.
 * @details Acceleration is a small value type with meters-per-second-squared and standard
 * gravity (g₀ = 9.80665 m/s²) conversions. It relates to Velocity through `std::chrono`
 * durations.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when wcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace whs31::literals;
 *
 * using namespace std::chrono_literals;
 *
 * auto gravity = 1_gn;            // 9.80665 m/s^2
 * auto fall_speed = gravity * 2s; // 19.6133 m/s
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Velocity
 * @see Force
 */
class WHS31_COMMON_API Acceleration : public QuantityBase<Acceleration>
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double metersPerSecondSquared READ as_mps2 CONSTANT FINAL)
  Q_PROPERTY(double standardGravity READ as_standard_gravity CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "m/s^2";

  /**
   * @brief Creates a zero acceleration.
   */
  constexpr Acceleration() noexcept = default;

  /**
   * @brief Creates an acceleration from meters per second squared.
   */
  explicit constexpr Acceleration(double const mps2) noexcept
    : QuantityBase(mps2)
  {}

  /**
   * @brief Creates an acceleration from meters per second squared.
   */
  [[nodiscard]] static constexpr Acceleration from_mps2(double const mps2) noexcept
  {
    return Acceleration(mps2);
  }

  /**
   * @brief Creates an acceleration from multiples of standard gravity.
   */
  [[nodiscard]] static constexpr Acceleration from_standard_gravity(
    double const standard_gravity
  ) noexcept
  {
    return Acceleration(standard_gravity * STANDARD_GRAVITY);
  }

  /**
   * @brief Returns meters per second squared.
   */
  [[nodiscard]] constexpr double as_mps2() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns multiples of standard gravity.
   */
  [[nodiscard]] constexpr double as_standard_gravity() const noexcept
  {
    return this->magnitude() / STANDARD_GRAVITY;
  }
};

/**
 * @brief Returns the average acceleration that changes a velocity by @p delta in @p time.
 * @ingroup core_measurements
 * @relates Acceleration
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr Acceleration operator/(
  Velocity const delta,
  std::chrono::duration<Rep, Period> const time
) noexcept
{
  return Acceleration::from_mps2(delta.as_mps() / std::chrono::duration<double>(time).count());
}

/**
 * @brief Returns the change of a velocity at @p rate during @p time.
 * @ingroup core_measurements
 * @relates Acceleration
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr Velocity operator*(
  Acceleration const rate,
  std::chrono::duration<Rep, Period> const time
) noexcept
{
  return Velocity::from_mps(rate.as_mps2() * std::chrono::duration<double>(time).count());
}

/**
 * @brief Returns the change of a velocity at @p rate during @p time.
 * @ingroup core_measurements
 * @relates Acceleration
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr Velocity operator*(
  std::chrono::duration<Rep, Period> const time,
  Acceleration const rate
) noexcept
{
  return rate * time;
}

inline namespace literals
{
  /**
   * @brief Creates an acceleration from a meters-per-second-squared literal.
   * @ingroup core_measurements
   * @relates Acceleration
   */
  [[nodiscard]] constexpr Acceleration operator""_mps2(long double const value) noexcept
  {
    return Acceleration::from_mps2(static_cast<double>(value));
  }

  /**
   * @brief Creates an acceleration from a meters-per-second-squared literal.
   * @ingroup core_measurements
   * @relates Acceleration
   */
  [[nodiscard]] constexpr Acceleration operator""_mps2(unsigned long long const value) noexcept
  {
    return Acceleration::from_mps2(static_cast<double>(value));
  }

  /**
   * @brief Creates an acceleration from a standard-gravity literal.
   * @ingroup core_measurements
   * @relates Acceleration
   */
  [[nodiscard]] constexpr Acceleration operator""_gn(long double const value) noexcept
  {
    return Acceleration::from_standard_gravity(static_cast<double>(value));
  }

  /**
   * @brief Creates an acceleration from a standard-gravity literal.
   * @ingroup core_measurements
   * @relates Acceleration
   */
  [[nodiscard]] constexpr Acceleration operator""_gn(unsigned long long const value) noexcept
  {
    return Acceleration::from_standard_gravity(static_cast<double>(value));
  }
}  // namespace literals

WHS31_COMMON_NAMESPACE_END

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Acceleration)
#endif
