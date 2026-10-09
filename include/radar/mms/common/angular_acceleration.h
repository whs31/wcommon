#pragma once

#include <chrono>
#include <string_view>

#include <radar/mms/common/angular_velocity.h>
#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/math.h>
#include <radar/mms/common/quantity.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Angular acceleration value stored in `rad/s^2`.
 * @details AngularAcceleration is a small value type with radian and degree rates. It
 * relates to AngularVelocity through `std::chrono` durations.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * using namespace std::chrono_literals;
 *
 * auto spin_up = 10_rad_per_s2 * 3s; // 30 rad/s
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see AngularVelocity
 * @see Torque
 * @see MomentOfInertia
 */
class RADAR_COMMON_API AngularAcceleration : public QuantityBase<AngularAcceleration>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double radiansPerSecondSquared READ as_radians_per_second_squared CONSTANT FINAL)
  Q_PROPERTY(double degreesPerSecondSquared READ as_degrees_per_second_squared CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "rad/s^2";

  /**
   * @brief Creates a zero angular acceleration.
   */
  constexpr AngularAcceleration() noexcept = default;

  /**
   * @brief Creates an angular acceleration from radians per second squared.
   */
  explicit constexpr AngularAcceleration(double const radians_per_second_squared) noexcept
    : QuantityBase(radians_per_second_squared)
  {}

  /**
   * @brief Creates an angular acceleration from radians per second squared.
   */
  [[nodiscard]] static constexpr AngularAcceleration from_radians_per_second_squared(
    double const radians_per_second_squared
  ) noexcept
  {
    return AngularAcceleration(radians_per_second_squared);
  }

  /**
   * @brief Creates an angular acceleration from degrees per second squared.
   */
  [[nodiscard]] static constexpr AngularAcceleration from_degrees_per_second_squared(
    double const degrees_per_second_squared
  ) noexcept
  {
    return AngularAcceleration(to_radians(degrees_per_second_squared));
  }

  /**
   * @brief Returns radians per second squared.
   */
  [[nodiscard]] constexpr double as_radians_per_second_squared() const noexcept
  {
    return this->magnitude();
  }

  /**
   * @brief Returns degrees per second squared.
   */
  [[nodiscard]] constexpr double as_degrees_per_second_squared() const noexcept
  {
    return to_degrees(this->magnitude());
  }
};

/**
 * @brief Returns the average angular acceleration that changes an angular velocity by @p delta in
 * @p time.
 * @ingroup core_measurements
 * @relates AngularAcceleration
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr AngularAcceleration operator/(
  AngularVelocity const delta,
  std::chrono::duration<Rep, Period> const time
) noexcept
{
  return AngularAcceleration::from_radians_per_second_squared(
    delta.as_radians_per_second() / std::chrono::duration<double>(time).count()
  );
}

/**
 * @brief Returns the change of an angular velocity at @p rate during @p time.
 * @ingroup core_measurements
 * @relates AngularAcceleration
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr AngularVelocity operator*(
  AngularAcceleration const rate,
  std::chrono::duration<Rep, Period> const time
) noexcept
{
  return AngularVelocity::from_radians_per_second(
    rate.as_radians_per_second_squared() * std::chrono::duration<double>(time).count()
  );
}

/**
 * @brief Returns the change of an angular velocity at @p rate during @p time.
 * @ingroup core_measurements
 * @relates AngularAcceleration
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr AngularVelocity operator*(
  std::chrono::duration<Rep, Period> const time,
  AngularAcceleration const rate
) noexcept
{
  return rate * time;
}

inline namespace literals
{
  /**
   * @brief Creates an angular acceleration from a radian-per-second-squared literal.
   * @ingroup core_measurements
   * @relates AngularAcceleration
   */
  [[nodiscard]] constexpr AngularAcceleration operator""_rad_per_s2(
    long double const value
  ) noexcept
  {
    return AngularAcceleration::from_radians_per_second_squared(static_cast<double>(value));
  }

  /**
   * @brief Creates an angular acceleration from a radian-per-second-squared literal.
   * @ingroup core_measurements
   * @relates AngularAcceleration
   */
  [[nodiscard]] constexpr AngularAcceleration operator""_rad_per_s2(
    unsigned long long const value
  ) noexcept
  {
    return AngularAcceleration::from_radians_per_second_squared(static_cast<double>(value));
  }

  /**
   * @brief Creates an angular acceleration from a degree-per-second-squared literal.
   * @ingroup core_measurements
   * @relates AngularAcceleration
   */
  [[nodiscard]] constexpr AngularAcceleration operator""_deg_per_s2(
    long double const value
  ) noexcept
  {
    return AngularAcceleration::from_degrees_per_second_squared(static_cast<double>(value));
  }

  /**
   * @brief Creates an angular acceleration from a degree-per-second-squared literal.
   * @ingroup core_measurements
   * @relates AngularAcceleration
   */
  [[nodiscard]] constexpr AngularAcceleration operator""_deg_per_s2(
    unsigned long long const value
  ) noexcept
  {
    return AngularAcceleration::from_degrees_per_second_squared(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::AngularAcceleration)
#endif
