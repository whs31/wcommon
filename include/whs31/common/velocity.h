#pragma once

#include <chrono>
#include <string_view>

#include <whs31/common/config.h>
#include <whs31/common/constants.h>
#include <whs31/common/detail/export.h>
#include <whs31/common/detail/namespace.h>
#include <whs31/common/distance.h>
#include <whs31/common/quantity.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Velocity value stored in meters per second.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when wcommon is built with `with_qt=True`.
 * @details Velocity is a small value type with conversions for meters per
 * second, kilometers per hour, and miles per hour. It relates to Distance
 * through `std::chrono` durations.
 *
 * @code {.cpp}
 * using namespace std::chrono_literals;
 * using namespace whs31::literals;
 *
 * auto cruise = whs31::common::Velocity::from_kmph(90.0);
 * auto text = cruise.to_string();  // "25 m/s"
 * auto travelled = cruise * 2s;    // 50 m
 * auto speed = 100_m / 4s;         // 25 m/s
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Distance
 * @see Acceleration
 */
class WHS31_COMMON_API Velocity : public QuantityBase<Velocity>
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double metersPerSecond READ as_mps CONSTANT FINAL)
  Q_PROPERTY(double kilometersPerHour READ as_kmph CONSTANT FINAL)
  Q_PROPERTY(double milesPerHour READ as_mph CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "m/s";

  /**
   * @brief Creates a zero velocity.
   */
  constexpr Velocity() noexcept = default;

  /**
   * @brief Creates a velocity from meters per second.
   */
  explicit constexpr Velocity(double const meters_per_second) noexcept
    : QuantityBase(meters_per_second)
  {}

  /**
   * @brief Creates a velocity from meters per second.
   */
  [[nodiscard]] static constexpr Velocity from_mps(double const meters_per_second) noexcept
  {
    return Velocity(meters_per_second);
  }

  /**
   * @brief Creates a velocity from kilometers per hour.
   */
  [[nodiscard]] static constexpr Velocity from_kmph(double const kilometers_per_hour) noexcept
  {
    return from_mps((kilometers_per_hour / METER_KILOMETER_FACTOR) / SECONDS_HOURS_FACTOR);
  }

  /**
   * @brief Creates a velocity from miles per hour.
   */
  [[nodiscard]] static constexpr Velocity from_mph(double const miles_per_hour) noexcept
  {
    return from_mps((miles_per_hour / METER_MILE_FACTOR) / SECONDS_HOURS_FACTOR);
  }

  /**
   * @brief Returns meters per second.
   */
  [[nodiscard]] constexpr double as_mps() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns kilometers per hour.
   */
  [[nodiscard]] constexpr double as_kmph() const noexcept
  {
    return (this->magnitude() * METER_KILOMETER_FACTOR) * SECONDS_HOURS_FACTOR;
  }

  /**
   * @brief Returns miles per hour.
   */
  [[nodiscard]] constexpr double as_mph() const noexcept
  {
    return this->magnitude() * METER_MILE_FACTOR * SECONDS_HOURS_FACTOR;
  }

  using QuantityBase::operator+=;
  using QuantityBase::operator-=;

  constexpr Velocity& operator+=(double const meters_per_second) noexcept
  {
    return *this += Velocity(meters_per_second);
  }

  constexpr Velocity& operator-=(double const meters_per_second) noexcept
  {
    return *this -= Velocity(meters_per_second);
  }
};

[[nodiscard]] constexpr Velocity operator+(Velocity lhs, double const meters_per_second) noexcept
{
  lhs += meters_per_second;
  return lhs;
}

[[nodiscard]] constexpr Velocity operator+(
  double const meters_per_second,
  Velocity const rhs
) noexcept
{
  return Velocity(meters_per_second) + rhs;
}

[[nodiscard]] constexpr Velocity operator-(Velocity lhs, double const meters_per_second) noexcept
{
  lhs -= meters_per_second;
  return lhs;
}

[[nodiscard]] constexpr Velocity operator-(
  double const meters_per_second,
  Velocity const rhs
) noexcept
{
  return Velocity(meters_per_second) - rhs;
}

/**
 * @brief Returns the average velocity that covers @p distance in @p time.
 * @ingroup core_measurements
 * @relates Velocity
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr Velocity operator/(
  Distance const distance,
  std::chrono::duration<Rep, Period> const time
) noexcept
{
  return Velocity::from_mps(distance.as_m() / std::chrono::duration<double>(time).count());
}

/**
 * @brief Returns the distance covered at @p velocity during @p time.
 * @ingroup core_measurements
 * @relates Velocity
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr Distance operator*(
  Velocity const velocity,
  std::chrono::duration<Rep, Period> const time
) noexcept
{
  return Distance::from_meters(velocity.as_mps() * std::chrono::duration<double>(time).count());
}

/**
 * @brief Returns the distance covered at @p velocity during @p time.
 * @ingroup core_measurements
 * @relates Velocity
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr Distance operator*(
  std::chrono::duration<Rep, Period> const time,
  Velocity const velocity
) noexcept
{
  return velocity * time;
}

/**
 * @brief Returns the time it takes to cover @p distance at @p velocity.
 * @ingroup core_measurements
 * @relates Velocity
 */
[[nodiscard]] constexpr std::chrono::duration<double> operator/(
  Distance const distance,
  Velocity const velocity
) noexcept
{
  return std::chrono::duration<double>(distance.as_m() / velocity.as_mps());
}

inline namespace literals
{
  /**
   * @brief Creates a velocity from a meters-per-second literal.
   * @ingroup core_measurements
   * @relates Velocity
   */
  [[nodiscard]] constexpr Velocity operator""_mps(long double const value) noexcept
  {
    return Velocity::from_mps(static_cast<double>(value));
  }

  /**
   * @brief Creates a velocity from a meters-per-second literal.
   * @ingroup core_measurements
   * @relates Velocity
   */
  [[nodiscard]] constexpr Velocity operator""_mps(unsigned long long const value) noexcept
  {
    return Velocity::from_mps(static_cast<double>(value));
  }

  /**
   * @brief Creates a velocity from a kilometers-per-hour literal.
   * @ingroup core_measurements
   * @relates Velocity
   */
  [[nodiscard]] constexpr Velocity operator""_kmph(long double const value) noexcept
  {
    return Velocity::from_kmph(static_cast<double>(value));
  }

  /**
   * @brief Creates a velocity from a kilometers-per-hour literal.
   * @ingroup core_measurements
   * @relates Velocity
   */
  [[nodiscard]] constexpr Velocity operator""_kmph(unsigned long long const value) noexcept
  {
    return Velocity::from_kmph(static_cast<double>(value));
  }

  /**
   * @brief Creates a velocity from a miles-per-hour literal.
   * @ingroup core_measurements
   * @relates Velocity
   */
  [[nodiscard]] constexpr Velocity operator""_mph(long double const value) noexcept
  {
    return Velocity::from_mph(static_cast<double>(value));
  }

  /**
   * @brief Creates a velocity from a miles-per-hour literal.
   * @ingroup core_measurements
   * @relates Velocity
   */
  [[nodiscard]] constexpr Velocity operator""_mph(unsigned long long const value) noexcept
  {
    return Velocity::from_mph(static_cast<double>(value));
  }
}  // namespace literals

WHS31_COMMON_NAMESPACE_END

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Velocity)
#endif
