#pragma once

#include <cmath>
#include <compare>
#include <functional>
#include <iosfwd>
#include <ostream>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <whs31/common/config.h>
#include <whs31/common/detail/export.h>
#include <whs31/common/detail/namespace.h>
#include <whs31/common/vector2.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qmetatype.h>
#  include <qmetaobject.h>
#  include <qpoint.h>
#  include <qstring.h>
#  include <qvector2d.h>
#endif

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Two-dimensional point backed by doubles.
 * @details Point2 represents a position. Subtracting two points yields a
 * Vector2; adding or subtracting a vector translates the point.
 * @note Qt properties, conversions, and metatype registration are available
 * only when wcommon is built with `with_qt=True`.
 *
 * @ingroup core_measurements
 * @see Vector2
 * @see Point3
 */
class WHS31_COMMON_API Point2
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double x READ x CONSTANT FINAL)
  Q_PROPERTY(double y READ y CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  constexpr Point2() noexcept = default;

  constexpr Point2(double const x, double const y) noexcept
    : m_x(x)
    , m_y(y)
  {}

  explicit constexpr Point2(Vector2 const vector) noexcept
    : m_x(vector.x())
    , m_y(vector.y())
  {}

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  explicit Point2(QPointF point) noexcept;
#endif

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  explicit Point2(QVector2D const& vector) noexcept;
#endif

  [[nodiscard]] constexpr double x() const noexcept { return this->m_x; }

  [[nodiscard]] constexpr double y() const noexcept { return this->m_y; }

  [[nodiscard]] constexpr Vector2 to_vector2() const noexcept { return {this->m_x, this->m_y}; }

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] QPointF to_qpointf() const noexcept;
#endif

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] QVector2D to_qvector2d() const noexcept;
#endif

  [[nodiscard]] double distance_to(Point2 const& other) const noexcept
  {
    return Vector2(other.m_x - this->m_x, other.m_y - this->m_y).length();
  }

  [[nodiscard]] Angle angle_to(Point2 const& other) const noexcept
  {
    return Vector2(other.m_x - this->m_x, other.m_y - this->m_y).angle();
  }

  [[nodiscard]] std::string to_string() const;

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] static constexpr Point2 zero() noexcept { return {}; }

  [[nodiscard]] static constexpr Point2 from_xy(double const x, double const y) noexcept
  {
    return {x, y};
  }

  [[nodiscard]] static constexpr Point2 from_vector2(Vector2 const vector) noexcept
  {
    return Point2(vector);
  }

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] static Point2 from_qpointf(QPointF point) noexcept;
#endif

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] static Point2 from_qvector2d(QVector2D const& vector) noexcept;
#endif

  [[nodiscard]] constexpr bool operator==(Point2 const&) const noexcept = default;

  [[nodiscard]] constexpr std::partial_ordering operator<=>(Point2 const&) const noexcept = default;

  constexpr Point2& operator+=(Vector2 const rhs) noexcept
  {
    this->m_x += rhs.x();
    this->m_y += rhs.y();
    return *this;
  }

  constexpr Point2& operator-=(Vector2 const rhs) noexcept
  {
    this->m_x -= rhs.x();
    this->m_y -= rhs.y();
    return *this;
  }

 private:
  double m_x = 0.0;
  double m_y = 0.0;
};

[[nodiscard]] constexpr Point2 operator+(Point2 lhs, Vector2 const rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Point2 operator+(Vector2 const lhs, Point2 rhs) noexcept
{
  rhs += lhs;
  return rhs;
}

[[nodiscard]] constexpr Point2 operator-(Point2 lhs, Vector2 const rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Vector2 operator-(Point2 const lhs, Point2 const rhs) noexcept
{
  return {lhs.x() - rhs.x(), lhs.y() - rhs.y()};
}

WHS31_COMMON_API std::ostream& operator<<(std::ostream& os, Point2 point);

WHS31_COMMON_NAMESPACE_END

template <>
struct std::hash<whs31::common::Point2>
{
  [[nodiscard]] std::size_t operator()(whs31::common::Point2 const point) const noexcept
  {
    auto hash = std::hash<double> {}(point.x());
    hash ^= std::hash<double> {}(point.y()) + 0x9e3779b9u + (hash << 6) + (hash >> 2);
    return hash;
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<whs31::common::Point2> : fmt::ostream_formatter
{};
#endif

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Point2)
#endif
