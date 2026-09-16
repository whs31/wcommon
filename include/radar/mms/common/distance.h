#pragma once

#include <cmath>
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
 * @brief Distance value stored in meters.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 * @details Distance is a small value type with common metric and imperial
 * conversions. Arithmetic combines the stored meter values directly, matching
 * the lightweight Rust type this class replaces.
 *
 * @code {.cpp}
 * auto lap = radar::mms::common::Distance::from_kilometers(1.2F);
 * auto total = lap + radar::mms::common::Distance::from_meters(300.0F);
 * fmt::print("{}\n", total); // "1.5 km"
 * @endcode
 *
 * @ingroup core_measurements
 * @see Velocity
 */
class RADAR_COMMON_API Distance
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(float nanometers READ as_nm CONSTANT FINAL)
  Q_PROPERTY(float micrometers READ as_um CONSTANT FINAL)
  Q_PROPERTY(float millimeters READ as_mm CONSTANT FINAL)
  Q_PROPERTY(float centimeters READ as_cm CONSTANT FINAL)
  Q_PROPERTY(float decimeters READ as_dm CONSTANT FINAL)
  Q_PROPERTY(float meters READ as_m CONSTANT FINAL)
  Q_PROPERTY(float hectometers READ as_hm CONSTANT FINAL)
  Q_PROPERTY(float kilometers READ as_km CONSTANT FINAL)
  Q_PROPERTY(float inches READ as_inches CONSTANT FINAL)
  Q_PROPERTY(float feet READ as_feet CONSTANT FINAL)
  Q_PROPERTY(float yards READ as_yards CONSTANT FINAL)
  Q_PROPERTY(float furlongs READ as_furlongs CONSTANT FINAL)
  Q_PROPERTY(float miles READ as_miles CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Creates a zero distance.
   */
  constexpr Distance() noexcept = default;

  /**
   * @brief Creates a distance from meters.
   */
  explicit constexpr Distance(float const meters) noexcept
    : m_meters(meters)
  {}

  /**
   * @brief Creates a distance from meters.
   */
  [[nodiscard]] static constexpr Distance new_distance(float const meters) noexcept
  {
    return Distance(meters);
  }

  /**
   * @brief Creates a distance from meters.
   */
  [[nodiscard]] static constexpr Distance from_meters(float const meters) noexcept
  {
    return Distance(meters);
  }

  [[nodiscard]] static constexpr Distance from_nanometers(float const nanometers) noexcept
  {
    return from_meters(nanometers / METER_NANOMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_micrometers(float const micrometers) noexcept
  {
    return from_meters(micrometers / METER_MICROMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_millimeters(float const millimeters) noexcept
  {
    return from_meters(millimeters / METER_MILLIMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_centimeters(float const centimeters) noexcept
  {
    return from_meters(centimeters / METER_CENTIMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_decimeters(float const decimeters) noexcept
  {
    return from_meters(decimeters / METER_DECIMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_hectometers(float const hectometers) noexcept
  {
    return from_meters(hectometers / METER_HECTOMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_kilometers(float const kilometers) noexcept
  {
    return from_meters(kilometers / METER_KILOMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_inches(float const inches) noexcept
  {
    return from_meters(inches / METER_INCH_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_feet(float const feet) noexcept
  {
    return from_meters(feet / METER_FEET_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_yards(float const yards) noexcept
  {
    return from_meters(yards / METER_YARD_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_furlongs(float const furlongs) noexcept
  {
    return from_meters(furlongs / METER_FURLONG_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_miles(float const miles) noexcept
  {
    return from_meters(miles / METER_MILE_FACTOR);
  }

  [[nodiscard]] constexpr float as_nm() const noexcept
  {
    return this->m_meters * METER_NANOMETER_FACTOR;
  }

  [[nodiscard]] constexpr float as_um() const noexcept
  {
    return this->m_meters * METER_MICROMETER_FACTOR;
  }

  [[nodiscard]] constexpr float as_mm() const noexcept
  {
    return this->m_meters * METER_MILLIMETER_FACTOR;
  }

  [[nodiscard]] constexpr float as_cm() const noexcept
  {
    return this->m_meters * METER_CENTIMETER_FACTOR;
  }

  [[nodiscard]] constexpr float as_dm() const noexcept
  {
    return this->m_meters * METER_DECIMETER_FACTOR;
  }

  [[nodiscard]] constexpr float as_m() const noexcept { return this->m_meters; }

  [[nodiscard]] constexpr float as_hm() const noexcept
  {
    return this->m_meters * METER_HECTOMETER_FACTOR;
  }

  [[nodiscard]] constexpr float as_km() const noexcept
  {
    return this->m_meters * METER_KILOMETER_FACTOR;
  }

  [[nodiscard]] constexpr float as_inches() const noexcept
  {
    return this->m_meters * METER_INCH_FACTOR;
  }

  [[nodiscard]] constexpr float as_feet() const noexcept
  {
    return this->m_meters * METER_FEET_FACTOR;
  }

  [[nodiscard]] constexpr float as_yards() const noexcept
  {
    return this->m_meters * METER_YARD_FACTOR;
  }

  [[nodiscard]] constexpr float as_furlongs() const noexcept
  {
    return this->m_meters * METER_FURLONG_FACTOR;
  }

  [[nodiscard]] constexpr float as_miles() const noexcept
  {
    return this->m_meters * METER_MILE_FACTOR;
  }

  /**
   * @brief Returns a compact string using millimeters, centimeters, meters, or kilometers.
   */
  [[nodiscard]] std::string to_string() const;

  /**
   * @brief Returns a compact Qt string representation.
   */
#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] constexpr explicit operator float() const noexcept { return this->m_meters; }

  [[nodiscard]] constexpr explicit operator double() const noexcept { return this->m_meters; }

  [[nodiscard]] constexpr bool operator==(Distance const&) const noexcept = default;

  [[nodiscard]] constexpr std::partial_ordering operator<=>(Distance const&) const noexcept =
    default;

  [[nodiscard]] constexpr Distance operator+() const noexcept { return *this; }

  [[nodiscard]] constexpr Distance operator-() const noexcept { return Distance(-this->m_meters); }

  constexpr Distance& operator+=(Distance const rhs) noexcept
  {
    this->m_meters += rhs.m_meters;
    return *this;
  }

  constexpr Distance& operator+=(float const rhs) noexcept
  {
    this->m_meters += rhs;
    return *this;
  }

  constexpr Distance& operator-=(Distance const rhs) noexcept
  {
    this->m_meters -= rhs.m_meters;
    return *this;
  }

  constexpr Distance& operator-=(float const rhs) noexcept
  {
    this->m_meters -= rhs;
    return *this;
  }

  constexpr Distance& operator*=(float const rhs) noexcept
  {
    this->m_meters *= rhs;
    return *this;
  }

  constexpr Distance& operator/=(float const rhs) noexcept
  {
    this->m_meters /= rhs;
    return *this;
  }

 private:
  float m_meters = 0.0F;
};

[[nodiscard]] constexpr Distance operator+(Distance lhs, Distance const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator+(Distance lhs, float const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator+(float const lhs, Distance const rhs) noexcept
{
  return Distance::from_meters(lhs + rhs.as_m());
}

[[nodiscard]] constexpr Distance operator-(Distance lhs, Distance const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator-(Distance lhs, float const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator-(float const lhs, Distance const rhs) noexcept
{
  return Distance::from_meters(lhs - rhs.as_m());
}

[[nodiscard]] constexpr Distance operator*(Distance lhs, float const rhs) noexcept
{
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator*(float const lhs, Distance const rhs) noexcept
{
  return Distance::from_meters(lhs * rhs.as_m());
}

[[nodiscard]] constexpr Distance operator/(Distance lhs, float const rhs) noexcept
{
  lhs /= rhs;
  return lhs;
}

/**
 * @brief Creates a distance from a meter literal.
 * @ingroup core_measurements
 * @relates Distance
 */
[[nodiscard]] constexpr Distance operator""_meters(long double const meters) noexcept
{
  return Distance::from_meters(static_cast<float>(meters));
}

/**
 * @brief Creates a distance from a meter literal.
 * @ingroup core_measurements
 * @relates Distance
 */
[[nodiscard]] constexpr Distance operator""_meters(unsigned long long const meters) noexcept
{
  return Distance::from_meters(static_cast<float>(meters));
}

/**
 * @brief Creates a distance from a kilometer literal.
 * @ingroup core_measurements
 * @relates Distance
 */
[[nodiscard]] constexpr Distance operator""_kilometers(long double const kilometers) noexcept
{
  return Distance::from_kilometers(static_cast<float>(kilometers));
}

/**
 * @brief Creates a distance from a kilometer literal.
 * @ingroup core_measurements
 * @relates Distance
 */
[[nodiscard]] constexpr Distance operator""_kilometers(unsigned long long const kilometers) noexcept
{
  return Distance::from_kilometers(static_cast<float>(kilometers));
}

RADAR_COMMON_API std::ostream& operator<<(std::ostream& os, Distance distance);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Distance>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Distance const distance) const noexcept
  {
    return std::hash<float> {}(distance.as_m());
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::Distance> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Distance)
#endif
