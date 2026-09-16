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
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/math.h>
#include <radar/mms/common/vector2.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetatype.h>
#  include <qmetaobject.h>
#  include <qpoint.h>
#  include <qstring.h>
#  include <qvector2d.h>
#  include <qvector3d.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Three-dimensional displacement vector backed by floats.
 * @note Qt properties, conversions, and metatype registration are available
 * only when rcommon is built with `with_qt=True`.
 * @ingroup core_measurements
 * @see Vector2
 * @see Point3
 */
class RADAR_COMMON_API Vector3
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(float x READ x CONSTANT FINAL)
  Q_PROPERTY(float y READ y CONSTANT FINAL)
  Q_PROPERTY(float z READ z CONSTANT FINAL)
  Q_PROPERTY(float length READ length CONSTANT FINAL)
  Q_PROPERTY(float lengthSquared READ length_squared CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  constexpr Vector3() noexcept = default;

  constexpr Vector3(float const x, float const y, float const z) noexcept
    : m_x(x)
    , m_y(y)
    , m_z(z)
  {}

  // The default z intentionally preserves the implicit Vector2-to-Vector3 conversion.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Vector3(Vector2 const vector, float const z = 0.0F) noexcept
    : m_x(vector.x())
    , m_y(vector.y())
    , m_z(z)
  {}

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  explicit Vector3(QPointF point) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  explicit Vector3(QVector2D const& vector) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  explicit Vector3(QVector3D const& vector) noexcept;
#endif

  [[nodiscard]] constexpr float x() const noexcept { return this->m_x; }

  [[nodiscard]] constexpr float y() const noexcept { return this->m_y; }

  [[nodiscard]] constexpr float z() const noexcept { return this->m_z; }

  [[nodiscard]] constexpr Vector2 xy() const noexcept { return {this->m_x, this->m_y}; }

  [[nodiscard]] constexpr float length_squared() const noexcept
  {
    return this->m_x * this->m_x + this->m_y * this->m_y + this->m_z * this->m_z;
  }

  [[nodiscard]] float length() const noexcept { return std::sqrt(this->length_squared()); }

  [[nodiscard]] constexpr float dot(Vector3 const& other) const noexcept
  {
    return this->m_x * other.m_x + this->m_y * other.m_y + this->m_z * other.m_z;
  }

  [[nodiscard]] constexpr Vector3 cross(Vector3 const& other) const noexcept
  {
    return {
      this->m_y * other.m_z - this->m_z * other.m_y,
      this->m_z * other.m_x - this->m_x * other.m_z,
      this->m_x * other.m_y - this->m_y * other.m_x
    };
  }

  [[nodiscard]] Vector3 normalized() const noexcept
  {
    auto const len = this->length();
    if(is_null(len))
      return {};

    auto result = *this;
    result /= len;
    return result;
  }

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QPointF to_qpointf() const noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QVector2D to_qvector2d() const noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QVector3D to_qvector3d() const noexcept;
#endif

  [[nodiscard]] std::string to_string() const;

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] static constexpr Vector3 zero() noexcept { return {}; }

  [[nodiscard]] static constexpr Vector3 splat(float const value) noexcept
  {
    return {value, value, value};
  }

  [[nodiscard]] static constexpr Vector3 from_xyz(
    float const x,
    float const y,
    float const z
  ) noexcept
  {
    return {x, y, z};
  }

  [[nodiscard]] static constexpr Vector3 from_vector2(
    Vector2 const vector,
    float const z = 0.0F
  ) noexcept
  {
    return {vector, z};
  }

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Vector3 from_qpointf(QPointF point) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Vector3 from_qvector2d(QVector2D const& vector) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Vector3 from_qvector3d(QVector3D const& vector) noexcept;
#endif

  [[nodiscard]] constexpr bool operator==(Vector3 const&) const noexcept = default;

  [[nodiscard]] constexpr std::partial_ordering operator<=>(Vector3 const&) const noexcept =
    default;

  [[nodiscard]] constexpr Vector3 operator+() const noexcept { return *this; }

  [[nodiscard]] constexpr Vector3 operator-() const noexcept
  {
    return {-this->m_x, -this->m_y, -this->m_z};
  }

  constexpr Vector3& operator+=(Vector3 const rhs) noexcept
  {
    this->m_x += rhs.m_x;
    this->m_y += rhs.m_y;
    this->m_z += rhs.m_z;
    return *this;
  }

  constexpr Vector3& operator-=(Vector3 const rhs) noexcept
  {
    this->m_x -= rhs.m_x;
    this->m_y -= rhs.m_y;
    this->m_z -= rhs.m_z;
    return *this;
  }

  constexpr Vector3& operator*=(float const rhs) noexcept
  {
    this->m_x *= rhs;
    this->m_y *= rhs;
    this->m_z *= rhs;
    return *this;
  }

  constexpr Vector3& operator/=(float const rhs) noexcept
  {
    this->m_x /= rhs;
    this->m_y /= rhs;
    this->m_z /= rhs;
    return *this;
  }

 private:
  float m_x = 0.0F;
  float m_y = 0.0F;
  float m_z = 0.0F;
};

[[nodiscard]] constexpr Vector3 operator+(Vector3 lhs, Vector3 const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Vector3 operator-(Vector3 lhs, Vector3 const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Vector3 operator*(Vector3 lhs, float const rhs) noexcept
{
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr Vector3 operator*(float const lhs, Vector3 const rhs) noexcept
{
  return rhs * lhs;
}

[[nodiscard]] constexpr Vector3 operator/(Vector3 lhs, float const rhs) noexcept
{
  lhs /= rhs;
  return lhs;
}

RADAR_COMMON_API std::ostream& operator<<(std::ostream& os, Vector3 vector);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Vector3>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Vector3 const vector) const noexcept
  {
    auto hash = std::hash<float> {}(vector.x());
    hash ^= std::hash<float> {}(vector.y()) + 0x9e3779b9u + (hash << 6) + (hash >> 2);
    hash ^= std::hash<float> {}(vector.z()) + 0x9e3779b9u + (hash << 6) + (hash >> 2);
    return hash;
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::Vector3> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Vector3)
#endif
