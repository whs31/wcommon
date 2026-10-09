#pragma once

#include <cmath>
#include <compare>
#include <functional>
#include <iosfwd>
#include <ostream>
#include <string>
#include <string_view>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <radar/mms/common/concepts.h>
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
 * auto lap = radar::mms::common::Distance::from_kilometers(1.2);
 * auto total = lap + radar::mms::common::Distance::from_meters(300.0);
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

  Q_PROPERTY(double nanometers READ as_nm CONSTANT FINAL)
  Q_PROPERTY(double micrometers READ as_um CONSTANT FINAL)
  Q_PROPERTY(double millimeters READ as_mm CONSTANT FINAL)
  Q_PROPERTY(double centimeters READ as_cm CONSTANT FINAL)
  Q_PROPERTY(double decimeters READ as_dm CONSTANT FINAL)
  Q_PROPERTY(double meters READ as_m CONSTANT FINAL)
  Q_PROPERTY(double hectometers READ as_hm CONSTANT FINAL)
  Q_PROPERTY(double kilometers READ as_km CONSTANT FINAL)
  Q_PROPERTY(double inches READ as_inches CONSTANT FINAL)
  Q_PROPERTY(double feet READ as_feet CONSTANT FINAL)
  Q_PROPERTY(double yards READ as_yards CONSTANT FINAL)
  Q_PROPERTY(double furlongs READ as_furlongs CONSTANT FINAL)
  Q_PROPERTY(double miles READ as_miles CONSTANT FINAL)
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
  explicit constexpr Distance(double const meters) noexcept
    : m_meters(meters)
  {}

  /**
   * @brief Creates a distance from meters.
   */
  [[nodiscard]] static constexpr Distance new_distance(double const meters) noexcept
  {
    return Distance(meters);
  }

  /**
   * @brief Creates a distance from meters.
   */
  [[nodiscard]] static constexpr Distance from_meters(double const meters) noexcept
  {
    return Distance(meters);
  }

  [[nodiscard]] static constexpr Distance from_nanometers(double const nanometers) noexcept
  {
    return from_meters(nanometers / METER_NANOMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_micrometers(double const micrometers) noexcept
  {
    return from_meters(micrometers / METER_MICROMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_millimeters(double const millimeters) noexcept
  {
    return from_meters(millimeters / METER_MILLIMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_centimeters(double const centimeters) noexcept
  {
    return from_meters(centimeters / METER_CENTIMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_decimeters(double const decimeters) noexcept
  {
    return from_meters(decimeters / METER_DECIMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_hectometers(double const hectometers) noexcept
  {
    return from_meters(hectometers / METER_HECTOMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_kilometers(double const kilometers) noexcept
  {
    return from_meters(kilometers / METER_KILOMETER_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_inches(double const inches) noexcept
  {
    return from_meters(inches / METER_INCH_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_feet(double const feet) noexcept
  {
    return from_meters(feet / METER_FEET_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_yards(double const yards) noexcept
  {
    return from_meters(yards / METER_YARD_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_furlongs(double const furlongs) noexcept
  {
    return from_meters(furlongs / METER_FURLONG_FACTOR);
  }

  [[nodiscard]] static constexpr Distance from_miles(double const miles) noexcept
  {
    return from_meters(miles / METER_MILE_FACTOR);
  }

  [[nodiscard]] constexpr double as_nm() const noexcept
  {
    return this->m_meters * METER_NANOMETER_FACTOR;
  }

  [[nodiscard]] constexpr double as_um() const noexcept
  {
    return this->m_meters * METER_MICROMETER_FACTOR;
  }

  [[nodiscard]] constexpr double as_mm() const noexcept
  {
    return this->m_meters * METER_MILLIMETER_FACTOR;
  }

  [[nodiscard]] constexpr double as_cm() const noexcept
  {
    return this->m_meters * METER_CENTIMETER_FACTOR;
  }

  [[nodiscard]] constexpr double as_dm() const noexcept
  {
    return this->m_meters * METER_DECIMETER_FACTOR;
  }

  [[nodiscard]] constexpr double as_m() const noexcept { return this->m_meters; }

  [[nodiscard]] constexpr double as_hm() const noexcept
  {
    return this->m_meters * METER_HECTOMETER_FACTOR;
  }

  [[nodiscard]] constexpr double as_km() const noexcept
  {
    return this->m_meters * METER_KILOMETER_FACTOR;
  }

  [[nodiscard]] constexpr double as_inches() const noexcept
  {
    return this->m_meters * METER_INCH_FACTOR;
  }

  [[nodiscard]] constexpr double as_feet() const noexcept
  {
    return this->m_meters * METER_FEET_FACTOR;
  }

  [[nodiscard]] constexpr double as_yards() const noexcept
  {
    return this->m_meters * METER_YARD_FACTOR;
  }

  [[nodiscard]] constexpr double as_furlongs() const noexcept
  {
    return this->m_meters * METER_FURLONG_FACTOR;
  }

  [[nodiscard]] constexpr double as_miles() const noexcept
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

  [[nodiscard]] constexpr explicit operator float() const noexcept
  {
    return static_cast<float>(this->m_meters);
  }

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

  constexpr Distance& operator+=(double const rhs) noexcept
  {
    this->m_meters += rhs;
    return *this;
  }

  constexpr Distance& operator-=(Distance const rhs) noexcept
  {
    this->m_meters -= rhs.m_meters;
    return *this;
  }

  constexpr Distance& operator-=(double const rhs) noexcept
  {
    this->m_meters -= rhs;
    return *this;
  }

  constexpr Distance& operator*=(double const rhs) noexcept
  {
    this->m_meters *= rhs;
    return *this;
  }

  constexpr Distance& operator/=(double const rhs) noexcept
  {
    this->m_meters /= rhs;
    return *this;
  }

 private:
  double m_meters = 0.0;
};

[[nodiscard]] constexpr Distance operator+(Distance lhs, Distance const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator+(Distance lhs, double const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator+(double const lhs, Distance const rhs) noexcept
{
  return Distance::from_meters(lhs + rhs.as_m());
}

[[nodiscard]] constexpr Distance operator-(Distance lhs, Distance const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator-(Distance lhs, double const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator-(double const lhs, Distance const rhs) noexcept
{
  return Distance::from_meters(lhs - rhs.as_m());
}

[[nodiscard]] constexpr Distance operator*(Distance lhs, double const rhs) noexcept
{
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr Distance operator*(double const lhs, Distance const rhs) noexcept
{
  return Distance::from_meters(lhs * rhs.as_m());
}

[[nodiscard]] constexpr Distance operator/(Distance lhs, double const rhs) noexcept
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
  return Distance::from_meters(static_cast<double>(meters));
}

/**
 * @brief Creates a distance from a meter literal.
 * @ingroup core_measurements
 * @relates Distance
 */
[[nodiscard]] constexpr Distance operator""_meters(unsigned long long const meters) noexcept
{
  return Distance::from_meters(static_cast<double>(meters));
}

/**
 * @brief Creates a distance from a kilometer literal.
 * @ingroup core_measurements
 * @relates Distance
 */
[[nodiscard]] constexpr Distance operator""_kilometers(long double const kilometers) noexcept
{
  return Distance::from_kilometers(static_cast<double>(kilometers));
}

/**
 * @brief Creates a distance from a kilometer literal.
 * @ingroup core_measurements
 * @relates Distance
 */
[[nodiscard]] constexpr Distance operator""_kilometers(unsigned long long const kilometers) noexcept
{
  return Distance::from_kilometers(static_cast<double>(kilometers));
}

/**
 * @brief Quantity traits of Distance: stored in `m`.
 * @ingroup core_measurements
 * @see Quantity
 */
template <>
struct QuantityTraits<Distance>
{
  static constexpr std::string_view si_symbol = "m";

  [[nodiscard]] static constexpr Distance from_si(double const meters) noexcept
  {
    return Distance::from_meters(meters);
  }
};

RADAR_COMMON_API std::ostream& operator<<(std::ostream& os, Distance distance);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Distance>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Distance const distance) const noexcept
  {
    return std::hash<double> {}(distance.as_m());
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
