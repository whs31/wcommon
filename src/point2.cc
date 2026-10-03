#include <radar/mms/common/point2.h>

#include <ostream>

#include <fmt/format.h>

RADAR_COMMON_NAMESPACE_BEGIN

std::string Point2::to_string() const { return fmt::format("({}, {})", this->m_x, this->m_y); }

#ifdef RCOMMON_WITH_QT
Point2::Point2(QPointF const point) noexcept
  : m_x(static_cast<double>(point.x()))
  , m_y(static_cast<double>(point.y()))
{}

Point2::Point2(QVector2D const& vector) noexcept
  : m_x(vector.x())
  , m_y(vector.y())
{}

QPointF Point2::to_qpointf() const noexcept { return {this->m_x, this->m_y}; }

QVector2D Point2::to_qvector2d() const noexcept
{
  return {static_cast<float>(this->m_x), static_cast<float>(this->m_y)};
}

QString Point2::to_qstring() const { return QString::fromStdString(this->to_string()); }

Point2 Point2::from_qpointf(QPointF const point) noexcept { return Point2(point); }

Point2 Point2::from_qvector2d(QVector2D const& vector) noexcept { return Point2(vector); }
#endif

std::ostream& operator<<(std::ostream& os, Point2 const point) { return os << point.to_string(); }

RADAR_COMMON_NAMESPACE_END
