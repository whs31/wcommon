#pragma once

#include <cmath>
#include <limits>
#include <numbers>
#include <string>
#include <string_view>

#include <radar/mms/common/concepts.h>
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
 * @brief Angle value stored in radians.
 * @details Angle is a small value type with degree/radian conversions,
 * wrapping helpers, interpolation along the shortest path, and Qt/QML
 * properties.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * auto heading = radar::mms::common::Angle::from_degrees(350.0);
 * auto target = radar::mms::common::Angle::from_degrees(10.0);
 * auto halfway = heading.lerp(target, 0.5); // 0 degrees
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see to_degrees
 * @see to_radians
 */
class RADAR_COMMON_API Angle : public QuantityBase<Angle>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double radians READ radians CONSTANT FINAL)
  Q_PROPERTY(double degrees READ degrees CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "rad";

  /**
   * @brief Creates a zero angle.
   */
  constexpr Angle() noexcept = default;

  /**
   * @brief Creates an angle from radians.
   */
  explicit constexpr Angle(double const radians) noexcept
    : QuantityBase(radians)
  {}

  /**
   * @brief Returns radians.
   */
  [[nodiscard]] constexpr double radians() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns degrees.
   */
  [[nodiscard]] constexpr double degrees() const noexcept { return to_degrees(this->magnitude()); }

  /**
   * @brief Returns the angle wrapped into `[0, 2*pi)`.
   */
  [[nodiscard]] constexpr Angle positive() const noexcept
  {
    auto constexpr two_pi = std::numbers::pi_v<double> * 2.0;
    auto radians = fmod(this->magnitude(), two_pi);
    if(radians < 0.0)
      radians += two_pi;
    if(approx_eq(radians, two_pi, std::numeric_limits<double>::epsilon() * 3.0))
      radians = 0.0;
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
    if(is_null(ret.radians(), std::numeric_limits<double>::epsilon()))
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
    auto constexpr max = std::numbers::pi_v<double> * 2.0;
    auto const d = fmod(other.radians() - this->radians(), max);
    return from_radians(fmod(2.0 * d, max) - d);
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
    delta *= static_cast<double>(t);

    auto result = *this;
    result += delta;
    return result;
  }

  [[nodiscard]] double sin() const noexcept { return std::sin(this->magnitude()); }

  [[nodiscard]] double cos() const noexcept { return std::cos(this->magnitude()); }

  [[nodiscard]] double tan() const noexcept { return std::tan(this->magnitude()); }

  [[nodiscard]] double asin() const noexcept { return std::asin(this->magnitude()); }

  [[nodiscard]] double acos() const noexcept { return std::acos(this->magnitude()); }

  [[nodiscard]] double atan() const noexcept { return std::atan(this->magnitude()); }

  /**
   * @brief Returns a compact string in degrees.
   */
  // Replaces the SI display of QuantityBase::to_string().
  // NOLINTNEXTLINE(bugprone-derived-method-shadowing-base-method)
  [[nodiscard]] std::string to_string() const;

  /**
   * @brief Creates an angle from radians.
   */
  [[nodiscard]] static constexpr Angle from_radians(double const radians) noexcept
  {
    return Angle(radians);
  }

  /**
   * @brief Creates an angle from degrees.
   */
  [[nodiscard]] static constexpr Angle from_degrees(double const degrees) noexcept
  {
    return Angle(to_radians(degrees));
  }

  [[nodiscard]] static constexpr Angle pi() noexcept
  {
    return from_radians(std::numbers::pi_v<double>);
  }

  [[nodiscard]] static constexpr Angle tau() noexcept
  {
    return from_radians(std::numbers::pi_v<double> * 2.0);
  }

  [[nodiscard]] static constexpr Angle frac_pi_2() noexcept
  {
    return from_radians(std::numbers::pi_v<double> / 2.0);
  }

  [[nodiscard]] static constexpr Angle frac_pi_3() noexcept
  {
    return from_radians(std::numbers::pi_v<double> / 3.0);
  }

  [[nodiscard]] static constexpr Angle frac_pi_4() noexcept
  {
    return from_radians(std::numbers::pi_v<double> / 4.0);
  }

  using QuantityBase::operator+=;
  using QuantityBase::operator-=;

  constexpr Angle& operator+=(double const radians) noexcept { return *this += Angle(radians); }

  constexpr Angle& operator-=(double const radians) noexcept { return *this -= Angle(radians); }
};

[[nodiscard]] constexpr Angle operator+(Angle lhs, double const radians) noexcept
{
  lhs += radians;
  return lhs;
}

[[nodiscard]] constexpr Angle operator+(double const radians, Angle const rhs) noexcept
{
  return Angle(radians) + rhs;
}

[[nodiscard]] constexpr Angle operator-(Angle lhs, double const radians) noexcept
{
  lhs -= radians;
  return lhs;
}

[[nodiscard]] constexpr Angle operator-(double const radians, Angle const rhs) noexcept
{
  return Angle(radians) - rhs;
}

inline namespace literals
{
  /**
   * @brief Creates an angle from a degree literal.
   * @ingroup core_measurements
   * @relates Angle
   */
  [[nodiscard]] constexpr Angle operator""_deg(long double const value) noexcept
  {
    return Angle::from_degrees(static_cast<double>(value));
  }

  /**
   * @brief Creates an angle from a degree literal.
   * @ingroup core_measurements
   * @relates Angle
   */
  [[nodiscard]] constexpr Angle operator""_deg(unsigned long long const value) noexcept
  {
    return Angle::from_degrees(static_cast<double>(value));
  }

  /**
   * @brief Creates an angle from a radian literal.
   * @ingroup core_measurements
   * @relates Angle
   */
  [[nodiscard]] constexpr Angle operator""_rad(long double const value) noexcept
  {
    return Angle::from_radians(static_cast<double>(value));
  }

  /**
   * @brief Creates an angle from a radian literal.
   * @ingroup core_measurements
   * @relates Angle
   */
  [[nodiscard]] constexpr Angle operator""_rad(unsigned long long const value) noexcept
  {
    return Angle::from_radians(static_cast<double>(value));
  }
}  // namespace literals

/**
 * @brief Returns @p angle in degrees.
 * @ingroup core_math
 * @relates Angle
 */
[[nodiscard]] constexpr double to_degrees(Angle const angle) noexcept { return angle.degrees(); }

/**
 * @brief Returns @p angle in radians.
 * @ingroup core_math
 * @relates Angle
 */
[[nodiscard]] constexpr double to_radians(Angle const angle) noexcept { return angle.radians(); }

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Angle)
#endif
