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
#include <radar/mms/common/point2.h>
#include <radar/mms/common/vector3.h>

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
 * @brief Three-dimensional point backed by doubles.
 * @note Qt properties, conversions, and metatype registration are available
 * only when rcommon is built with `with_qt=True`.
 * @ingroup core_measurements
 * @see Point2
 * @see Vector3
 */
class RADAR_COMMON_API Point3
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double x READ x CONSTANT FINAL)
  Q_PROPERTY(double y READ y CONSTANT FINAL)
  Q_PROPERTY(double z READ z CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  constexpr Point3() noexcept = default;

  constexpr Point3(double const x, double const y, double const z) noexcept
    : m_x(x)
    , m_y(y)
    , m_z(z)
  {}

  // The default z intentionally preserves the implicit Point2-to-Point3 conversion.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Point3(Point2 const point, double const z = 0.0) noexcept
    : m_x(point.x())
    , m_y(point.y())
    , m_z(z)
  {}

  explicit constexpr Point3(Vector3 const vector) noexcept
    : m_x(vector.x())
    , m_y(vector.y())
    , m_z(vector.z())
  {}

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  explicit Point3(QPointF point) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  explicit Point3(QVector2D const& vector) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  explicit Point3(QVector3D const& vector) noexcept;
#endif

  [[nodiscard]] constexpr double x() const noexcept { return this->m_x; }

  [[nodiscard]] constexpr double y() const noexcept { return this->m_y; }

  [[nodiscard]] constexpr double z() const noexcept { return this->m_z; }

  [[nodiscard]] constexpr Point2 xy() const noexcept { return {this->m_x, this->m_y}; }

  [[nodiscard]] constexpr Vector3 to_vector3() const noexcept
  {
    return {this->m_x, this->m_y, this->m_z};
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

  [[nodiscard]] double distance_to(Point3 const& other) const noexcept
  {
    return Vector3(other.m_x - this->m_x, other.m_y - this->m_y, other.m_z - this->m_z).length();
  }

  [[nodiscard]] std::string to_string() const;

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] static constexpr Point3 zero() noexcept { return {}; }

  [[nodiscard]] static constexpr Point3 from_xyz(
    double const x,
    double const y,
    double const z
  ) noexcept
  {
    return {x, y, z};
  }

  [[nodiscard]] static constexpr Point3 from_point2(
    Point2 const point,
    double const z = 0.0
  ) noexcept
  {
    return {point, z};
  }

  [[nodiscard]] static constexpr Point3 from_vector3(Vector3 const vector) noexcept
  {
    return Point3(vector);
  }

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Point3 from_qpointf(QPointF point) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Point3 from_qvector2d(QVector2D const& vector) noexcept;
#endif

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Point3 from_qvector3d(QVector3D const& vector) noexcept;
#endif

  [[nodiscard]] constexpr bool operator==(Point3 const&) const noexcept = default;

  [[nodiscard]] constexpr std::partial_ordering operator<=>(Point3 const&) const noexcept = default;

  constexpr Point3& operator+=(Vector3 const rhs) noexcept
  {
    this->m_x += rhs.x();
    this->m_y += rhs.y();
    this->m_z += rhs.z();
    return *this;
  }

  constexpr Point3& operator-=(Vector3 const rhs) noexcept
  {
    this->m_x -= rhs.x();
    this->m_y -= rhs.y();
    this->m_z -= rhs.z();
    return *this;
  }

 private:
  double m_x = 0.0;
  double m_y = 0.0;
  double m_z = 0.0;
};

[[nodiscard]] constexpr Point3 operator+(Point3 lhs, Vector3 const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Point3 operator+(Vector3 const lhs, Point3 rhs) noexcept
{
  rhs += lhs;
  return rhs;
}

[[nodiscard]] constexpr Point3 operator-(Point3 lhs, Vector3 const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Vector3 operator-(Point3 const lhs, Point3 const rhs) noexcept
{
  return {lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z()};
}

RADAR_COMMON_API std::ostream& operator<<(std::ostream& os, Point3 point);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Point3>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Point3 const point) const noexcept
  {
    auto hash = std::hash<double> {}(point.x());
    hash ^= std::hash<double> {}(point.y()) + 0x9e3779b9u + (hash << 6) + (hash >> 2);
    hash ^= std::hash<double> {}(point.z()) + 0x9e3779b9u + (hash << 6) + (hash >> 2);
    return hash;
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::Point3> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Point3)
#endif
