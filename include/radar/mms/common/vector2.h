#pragma once

#include <cmath>
#include <compare>
#include <functional>
#include <iosfwd>
#include <ostream>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <radar/mms/common/angle.h>
#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/math.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetatype.h>
#  include <qmetaobject.h>
#  include <qpoint.h>
#  include <qstring.h>
#  include <qvector2d.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Two-dimensional displacement vector backed by floats.
 * @details The API follows the usual split from euclid-style geometry:
 * vectors represent offsets and directions, while Point2 represents positions.
 * @note Qt properties, conversions, and metatype registration are available
 * only when rcommon is built with `with_qt=True`.
 *
 * @ingroup core_measurements
 * @see Point2
 * @see Vector3
 * @see Angle
 */
class RADAR_COMMON_API Vector2
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(float x READ x CONSTANT FINAL)
  Q_PROPERTY(float y READ y CONSTANT FINAL)
  Q_PROPERTY(float length READ length CONSTANT FINAL)
  Q_PROPERTY(float lengthSquared READ length_squared CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  constexpr Vector2() noexcept = default;

  constexpr Vector2(float const x, float const y) noexcept
    : m_x(x)
    , m_y(y)
  {}

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  explicit Vector2(QPointF point) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  explicit Vector2(QVector2D const& vector) noexcept;
#endif

  [[nodiscard]] constexpr float x() const noexcept { return this->m_x; }

  [[nodiscard]] constexpr float y() const noexcept { return this->m_y; }

  [[nodiscard]] constexpr float length_squared() const noexcept
  {
    return this->m_x * this->m_x + this->m_y * this->m_y;
  }

  [[nodiscard]] float length() const noexcept { return std::sqrt(this->length_squared()); }

  [[nodiscard]] Angle angle() const noexcept
  {
    return Angle::from_radians(std::atan2(this->m_y, this->m_x));
  }

  [[nodiscard]] constexpr float dot(Vector2 const& other) const noexcept
  {
    return this->m_x * other.m_x + this->m_y * other.m_y;
  }

  [[nodiscard]] constexpr float cross(Vector2 const& other) const noexcept
  {
    return this->m_x * other.m_y - this->m_y * other.m_x;
  }

  [[nodiscard]] Angle angle_to(Vector2 const& other) const noexcept
  {
    return this->angle().angle_to(other.angle());
  }

  [[nodiscard]] Vector2 normalized() const noexcept
  {
    auto const len = this->length();
    if(is_null(len))
      return {};

    auto result = *this;
    result /= len;
    return result;
  }

  [[nodiscard]] Vector2 rotated(Angle const angle) const noexcept
  {
    auto const c = angle.cos();
    auto const s = angle.sin();
    return {this->m_x * c - this->m_y * s, this->m_x * s + this->m_y * c};
  }

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QPointF to_qpointf() const noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QVector2D to_qvector2d() const noexcept;
#endif

  [[nodiscard]] std::string to_string() const;

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] static constexpr Vector2 zero() noexcept { return {}; }

  [[nodiscard]] static constexpr Vector2 splat(float const value) noexcept
  {
    return {value, value};
  }

  [[nodiscard]] static constexpr Vector2 from_xy(float const x, float const y) noexcept
  {
    return {x, y};
  }

  [[nodiscard]] static Vector2 from_angle(Angle const angle) noexcept
  {
    return {angle.cos(), angle.sin()};
  }

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Vector2 from_qpointf(QPointF point) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Vector2 from_qvector2d(QVector2D const& vector) noexcept;
#endif

  [[nodiscard]] constexpr bool operator==(Vector2 const&) const noexcept = default;

  [[nodiscard]] constexpr std::partial_ordering operator<=>(Vector2 const&) const noexcept =
    default;

  [[nodiscard]] constexpr Vector2 operator+() const noexcept { return *this; }

  [[nodiscard]] constexpr Vector2 operator-() const noexcept { return {-this->m_x, -this->m_y}; }

  constexpr Vector2& operator+=(Vector2 const rhs) noexcept
  {
    this->m_x += rhs.m_x;
    this->m_y += rhs.m_y;
    return *this;
  }

  constexpr Vector2& operator-=(Vector2 const rhs) noexcept
  {
    this->m_x -= rhs.m_x;
    this->m_y -= rhs.m_y;
    return *this;
  }

  constexpr Vector2& operator*=(float const rhs) noexcept
  {
    this->m_x *= rhs;
    this->m_y *= rhs;
    return *this;
  }

  constexpr Vector2& operator/=(float const rhs) noexcept
  {
    this->m_x /= rhs;
    this->m_y /= rhs;
    return *this;
  }

 private:
  float m_x = 0.0F;
  float m_y = 0.0F;
};

[[nodiscard]] constexpr Vector2 operator+(Vector2 lhs, Vector2 const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Vector2 operator-(Vector2 lhs, Vector2 const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Vector2 operator*(Vector2 lhs, float const rhs) noexcept
{
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr Vector2 operator*(float const lhs, Vector2 const rhs) noexcept
{
  return rhs * lhs;
}

[[nodiscard]] constexpr Vector2 operator/(Vector2 lhs, float const rhs) noexcept
{
  lhs /= rhs;
  return lhs;
}

RADAR_COMMON_API std::ostream& operator<<(std::ostream& os, Vector2 vector);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Vector2>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Vector2 const vector) const noexcept
  {
    auto hash = std::hash<float> {}(vector.x());
    hash ^= std::hash<float> {}(vector.y()) + 0x9e3779b9u + (hash << 6) + (hash >> 2);
    return hash;
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::Vector2> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Vector2)
#endif
