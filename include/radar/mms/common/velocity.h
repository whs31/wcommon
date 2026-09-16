#pragma once

#include <compare>
#include <functional>
#include <iosfwd>
#include <ostream>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <radar/mms/common/config.h>
#include <radar/mms/common/constants.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Velocity value stored in meters per second.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 * @details Velocity is a small value type with conversions for meters per
 * second, kilometers per hour, and miles per hour.
 *
 * @code {.cpp}
 * auto cruise = radar::mms::common::Velocity::from_kmph(90.0F);
 * auto text = cruise.to_string(); // "25 m/s"
 * @endcode
 *
 * @ingroup core_measurements
 * @see Distance
 */
class RADAR_COMMON_API Velocity
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(float metersPerSecond READ as_mps CONSTANT FINAL)
  Q_PROPERTY(float kilometersPerHour READ as_kmph CONSTANT FINAL)
  Q_PROPERTY(float milesPerHour READ as_mph CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Creates a zero velocity.
   */
  constexpr Velocity() noexcept = default;

  /**
   * @brief Creates a velocity from meters per second.
   */
  explicit constexpr Velocity(float const meters_per_second) noexcept
    : m_mps(meters_per_second)
  {}

  /**
   * @brief Creates a velocity from meters per second.
   */
  [[nodiscard]] static constexpr Velocity from_mps(float const meters_per_second) noexcept
  {
    return Velocity(meters_per_second);
  }

  /**
   * @brief Creates a velocity from kilometers per hour.
   */
  [[nodiscard]] static constexpr Velocity from_kmph(float const kilometers_per_hour) noexcept
  {
    return from_mps((kilometers_per_hour / METER_KILOMETER_FACTOR) / SECONDS_HOURS_FACTOR);
  }

  /**
   * @brief Creates a velocity from miles per hour.
   */
  [[nodiscard]] static constexpr Velocity from_mph(float const miles_per_hour) noexcept
  {
    return from_mps((miles_per_hour / METER_MILE_FACTOR) / SECONDS_HOURS_FACTOR);
  }

  /**
   * @brief Returns meters per second.
   */
  [[nodiscard]] constexpr float as_mps() const noexcept { return this->m_mps; }

  /**
   * @brief Returns kilometers per hour.
   */
  [[nodiscard]] constexpr float as_kmph() const noexcept
  {
    return (this->m_mps * METER_KILOMETER_FACTOR) * SECONDS_HOURS_FACTOR;
  }

  /**
   * @brief Returns miles per hour.
   */
  [[nodiscard]] constexpr float as_mph() const noexcept
  {
    return this->m_mps * METER_MILE_FACTOR * SECONDS_HOURS_FACTOR;
  }

  /**
   * @brief Returns a string in meters per second.
   */
  [[nodiscard]] std::string to_string() const;

  /**
   * @brief Returns a Qt string in meters per second.
   */
#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] constexpr explicit operator float() const noexcept { return this->m_mps; }

  [[nodiscard]] constexpr explicit operator double() const noexcept { return this->m_mps; }

  [[nodiscard]] constexpr bool operator==(Velocity const&) const noexcept = default;

  [[nodiscard]] constexpr std::partial_ordering operator<=>(Velocity const&) const noexcept =
    default;

  [[nodiscard]] constexpr Velocity operator+() const noexcept { return *this; }

  [[nodiscard]] constexpr Velocity operator-() const noexcept { return Velocity(-this->m_mps); }

  constexpr Velocity& operator+=(Velocity const rhs) noexcept
  {
    this->m_mps += rhs.m_mps;
    return *this;
  }

  constexpr Velocity& operator+=(float const rhs) noexcept
  {
    this->m_mps += rhs;
    return *this;
  }

  constexpr Velocity& operator-=(Velocity const rhs) noexcept
  {
    this->m_mps -= rhs.m_mps;
    return *this;
  }

  constexpr Velocity& operator-=(float const rhs) noexcept
  {
    this->m_mps -= rhs;
    return *this;
  }

  constexpr Velocity& operator*=(float const rhs) noexcept
  {
    this->m_mps *= rhs;
    return *this;
  }

  constexpr Velocity& operator/=(float const rhs) noexcept
  {
    this->m_mps /= rhs;
    return *this;
  }

 private:
  float m_mps = 0.0F;
};

[[nodiscard]] constexpr Velocity operator+(Velocity lhs, Velocity const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Velocity operator+(Velocity lhs, float const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Velocity operator+(float const lhs, Velocity const rhs) noexcept
{
  return Velocity::from_mps(lhs + rhs.as_mps());
}

[[nodiscard]] constexpr Velocity operator-(Velocity lhs, Velocity const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Velocity operator-(Velocity lhs, float const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Velocity operator-(float const lhs, Velocity const rhs) noexcept
{
  return Velocity::from_mps(lhs - rhs.as_mps());
}

[[nodiscard]] constexpr Velocity operator*(Velocity lhs, float const rhs) noexcept
{
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr Velocity operator*(float const lhs, Velocity const rhs) noexcept
{
  return Velocity::from_mps(lhs * rhs.as_mps());
}

[[nodiscard]] constexpr Velocity operator/(Velocity lhs, float const rhs) noexcept
{
  lhs /= rhs;
  return lhs;
}

/**
 * @brief Creates a velocity from a meters-per-second literal.
 * @ingroup core_measurements
 * @relates Velocity
 */
[[nodiscard]] constexpr Velocity operator""_mps(long double const meters_per_second) noexcept
{
  return Velocity::from_mps(static_cast<float>(meters_per_second));
}

/**
 * @brief Creates a velocity from a meters-per-second literal.
 * @ingroup core_measurements
 * @relates Velocity
 */
[[nodiscard]] constexpr Velocity operator""_mps(unsigned long long const meters_per_second) noexcept
{
  return Velocity::from_mps(static_cast<float>(meters_per_second));
}

/**
 * @brief Creates a velocity from a kilometers-per-hour literal.
 * @ingroup core_measurements
 * @relates Velocity
 */
[[nodiscard]] constexpr Velocity operator""_kmph(long double const kilometers_per_hour) noexcept
{
  return Velocity::from_kmph(static_cast<float>(kilometers_per_hour));
}

/**
 * @brief Creates a velocity from a kilometers-per-hour literal.
 * @ingroup core_measurements
 * @relates Velocity
 */
[[nodiscard]] constexpr Velocity operator""_kmph(
  unsigned long long const kilometers_per_hour
) noexcept
{
  return Velocity::from_kmph(static_cast<float>(kilometers_per_hour));
}

/**
 * @brief Creates a velocity from a miles-per-hour literal.
 * @ingroup core_measurements
 * @relates Velocity
 */
[[nodiscard]] constexpr Velocity operator""_mph(long double const miles_per_hour) noexcept
{
  return Velocity::from_mph(static_cast<float>(miles_per_hour));
}

/**
 * @brief Creates a velocity from a miles-per-hour literal.
 * @ingroup core_measurements
 * @relates Velocity
 */
[[nodiscard]] constexpr Velocity operator""_mph(unsigned long long const miles_per_hour) noexcept
{
  return Velocity::from_mph(static_cast<float>(miles_per_hour));
}

RADAR_COMMON_API std::ostream& operator<<(std::ostream& os, Velocity velocity);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Velocity>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Velocity const velocity) const noexcept
  {
    return std::hash<float> {}(velocity.as_mps());
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::Velocity> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Velocity)
#endif
