#pragma once

#include <cmath>
#include <compare>
#include <functional>
#include <iosfwd>
#include <limits>
#include <ostream>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/error.h>
#include <radar/mms/common/math.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Angle value stored in radians.
 * @details Angle is a small value type with degree/radian conversions,
 * wrapping helpers, interpolation along the shortest path, and Qt/QML
 * properties.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * auto heading = radar::mms::common::Angle::from_degrees(350.0F);
 * auto target = radar::mms::common::Angle::from_degrees(10.0F);
 * auto halfway = heading.lerp(target, 0.5F); // 0 degrees
 * @endcode
 *
 * @ingroup core_measurements
 * @see to_degrees
 * @see to_radians
 */
class RADAR_COMMON_API Angle
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(float radians READ radians CONSTANT FINAL)
  Q_PROPERTY(float degrees READ degrees CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Creates a zero angle.
   */
  constexpr Angle() noexcept = default;

  /**
   * @brief Creates an angle from radians.
   */
  explicit constexpr Angle(float const radians) noexcept
    : m_radians(radians)
  {}

  /**
   * @brief Creates an angle from radians.
   */
  explicit constexpr Angle(double const radians) noexcept
    : m_radians(static_cast<float>(radians))
  {}

  constexpr Angle(Angle const&) = default;
  constexpr Angle(Angle&&) noexcept = default;
  constexpr Angle& operator=(Angle const&) = default;
  constexpr Angle& operator=(Angle&&) noexcept = default;
  ~Angle() = default;

  /**
   * @brief Returns radians.
   */
  [[nodiscard]] constexpr float radians() const noexcept { return this->m_radians; }

  /**
   * @brief Returns degrees.
   */
  [[nodiscard]] constexpr float degrees() const noexcept { return to_degrees(this->m_radians); }

  /**
   * @brief Returns the angle wrapped into `[0, 2*pi)`.
   */
  [[nodiscard]] constexpr Angle positive() const noexcept
  {
    constexpr auto two_pi = std::numbers::pi_v<float> * 2.0F;
    auto radians = fmod(this->m_radians, two_pi);
    if(radians < 0.0F)
      radians += two_pi;
    if(approx_eq(radians, two_pi, std::numeric_limits<float>::epsilon() * 3.0F))
      radians = 0.0F;
    return from_radians(radians);
  }

  /**
   * @brief Returns the equivalent signed angle in `[-pi, pi]`.
   */
  // The trailing underscore avoids the C++ keyword while preserving the public API.
  [[nodiscard]] constexpr Angle signed_() const noexcept  // NOLINT(readability-identifier-naming)
  {
    auto inner = pi();
    inner -= *this;
    inner = inner.positive();

    auto ret = pi();
    ret -= inner;
    if(is_null(ret.m_radians, std::numeric_limits<float>::epsilon()))
      return zero();
    return ret;
  }

  /**
   * @brief Returns the shortest signed angle from this angle to @p other.
   * @details The result accounts for wrapping and is suitable for steering or
   * heading interpolation.
   */
  [[nodiscard]] constexpr Angle angle_to(Angle const& other) const noexcept
  {
    constexpr auto max = std::numbers::pi_v<float> * 2.0F;
    auto const d = fmod(other.radians() - this->radians(), max);
    return from_radians(fmod(2.0F * d, max) - d);
  }

  /**
   * @brief Linearly interpolates toward @p other using the shortest path.
   * @param other Target angle.
   * @param t Interpolation factor.
   */
  template <Number T>
  [[nodiscard]] Angle lerp(Angle const& other, T const t) const noexcept
  {
    auto delta = this->angle_to(other);
    delta *= static_cast<float>(t);

    auto result = *this;
    result += delta;
    return result;
  }

  /**
   * @brief Checks whether the stored radians value is finite.
   */
  [[nodiscard]] bool is_finite() const noexcept { return std::isfinite(this->m_radians); }

  [[nodiscard]] float sin() const noexcept { return std::sin(this->m_radians); }

  [[nodiscard]] float cos() const noexcept { return std::cos(this->m_radians); }

  [[nodiscard]] float tan() const noexcept { return std::tan(this->m_radians); }

  [[nodiscard]] float asin() const noexcept { return std::asin(this->m_radians); }

  [[nodiscard]] float acos() const noexcept { return std::acos(this->m_radians); }

  [[nodiscard]] float atan() const noexcept { return std::atan(this->m_radians); }

  /**
   * @brief Returns a compact string in degrees.
   */
  [[nodiscard]] std::string to_string() const;

  /**
   * @brief Returns a compact Qt string in degrees.
   * @note Available only when rcommon is built with `with_qt=True`.
   */
#ifdef RCOMMON_WITH_QT
  [[nodiscard]] QString to_qstring() const;
#endif

  /**
   * @brief Creates an angle from radians.
   */
  [[nodiscard]] static constexpr Angle from_radians(float const radians) noexcept
  {
    return Angle(radians);
  }

  /**
   * @brief Creates an angle from degrees.
   */
  [[nodiscard]] static constexpr Angle from_degrees(float const degrees) noexcept
  {
    return Angle(to_radians(degrees));
  }

  [[nodiscard]] static constexpr Angle zero() noexcept { return {}; }

  [[nodiscard]] static constexpr Angle pi() noexcept
  {
    return from_radians(std::numbers::pi_v<float>);
  }

  [[nodiscard]] static constexpr Angle tau() noexcept
  {
    return from_radians(std::numbers::pi_v<float> * 2.0F);
  }

  [[nodiscard]] static constexpr Angle frac_pi_2() noexcept
  {
    return from_radians(std::numbers::pi_v<float> / 2.0F);
  }

  [[nodiscard]] static constexpr Angle frac_pi_3() noexcept
  {
    return from_radians(std::numbers::pi_v<float> / 3.0F);
  }

  [[nodiscard]] static constexpr Angle frac_pi_4() noexcept
  {
    return from_radians(std::numbers::pi_v<float> / 4.0F);
  }

  [[nodiscard]] constexpr explicit operator float() const noexcept { return this->m_radians; }

  [[nodiscard]] constexpr explicit operator double() const noexcept { return this->m_radians; }

  [[nodiscard]] constexpr bool operator==(Angle const&) const noexcept = default;

  [[nodiscard]] constexpr std::partial_ordering operator<=>(Angle const&) const noexcept = default;

  [[nodiscard]] constexpr Angle operator+() const noexcept { return *this; }

  [[nodiscard]] constexpr Angle operator-() const noexcept { return Angle(-this->m_radians); }

  constexpr Angle& operator+=(Angle const rhs) noexcept
  {
    this->m_radians += rhs.m_radians;
    return *this;
  }

  constexpr Angle& operator+=(float const rhs) noexcept
  {
    this->m_radians += rhs;
    return *this;
  }

  constexpr Angle& operator-=(Angle const rhs) noexcept
  {
    this->m_radians -= rhs.m_radians;
    return *this;
  }

  constexpr Angle& operator-=(float const rhs) noexcept
  {
    this->m_radians -= rhs;
    return *this;
  }

  constexpr Angle& operator*=(float const rhs) noexcept
  {
    this->m_radians *= rhs;
    return *this;
  }

  constexpr Angle& operator/=(float const rhs) noexcept
  {
    this->m_radians /= rhs;
    return *this;
  }

 private:
  float m_radians = 0.0F;
};

[[nodiscard]] constexpr Angle operator+(Angle lhs, Angle const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Angle operator+(Angle lhs, float const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Angle operator+(float const lhs, Angle const rhs) noexcept
{
  return Angle::from_radians(lhs + rhs.radians());
}

[[nodiscard]] constexpr Angle operator-(Angle lhs, Angle const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Angle operator-(Angle lhs, float const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Angle operator-(float const lhs, Angle const rhs) noexcept
{
  return Angle::from_radians(lhs - rhs.radians());
}

[[nodiscard]] constexpr Angle operator*(Angle lhs, float const rhs) noexcept
{
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr Angle operator*(float const lhs, Angle const rhs) noexcept
{
  return Angle::from_radians(lhs * rhs.radians());
}

[[nodiscard]] constexpr Angle operator/(Angle lhs, float const rhs) noexcept
{
  lhs /= rhs;
  return lhs;
}

/**
 * @brief Creates an angle from a degree literal.
 * @ingroup core_measurements
 * @relates Angle
 */
[[nodiscard]] constexpr Angle operator""_deg(long double const degrees) noexcept
{
  return Angle::from_degrees(static_cast<float>(degrees));
}

/**
 * @brief Creates an angle from a degree literal.
 * @ingroup core_measurements
 * @relates Angle
 */
[[nodiscard]] constexpr Angle operator""_deg(unsigned long long const degrees) noexcept
{
  return Angle::from_degrees(static_cast<float>(degrees));
}

/**
 * @brief Creates an angle from a radian literal.
 * @ingroup core_measurements
 * @relates Angle
 */
[[nodiscard]] constexpr Angle operator""_rad(long double const radians) noexcept
{
  return Angle::from_radians(static_cast<float>(radians));
}

/**
 * @brief Creates an angle from a radian literal.
 * @ingroup core_measurements
 * @relates Angle
 */
[[nodiscard]] constexpr Angle operator""_rad(unsigned long long const radians) noexcept
{
  return Angle::from_radians(static_cast<float>(radians));
}

/**
 * @brief Returns @p angle in degrees.
 * @ingroup core_math
 * @relates Angle
 */
[[nodiscard]] constexpr float to_degrees(Angle const angle) noexcept { return angle.degrees(); }

/**
 * @brief Returns @p angle in radians.
 * @ingroup core_math
 * @relates Angle
 */
[[nodiscard]] constexpr float to_radians(Angle const angle) noexcept { return angle.radians(); }

RADAR_COMMON_API std::ostream& operator<<(std::ostream& os, Angle angle);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Angle>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Angle const angle) const noexcept
  {
    return std::hash<float> {}(angle.radians());
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::Angle> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Angle)
#endif
