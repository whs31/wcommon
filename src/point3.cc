#include <radar/mms/common/point3.h>

#include <ostream>

#include <fmt/format.h>

RADAR_COMMON_NAMESPACE_BEGIN

std::string Point3::to_string() const
{
  return fmt::format("({}, {}, {})", this->m_x, this->m_y, this->m_z);
}

#ifdef RCOMMON_WITH_QT
Point3::Point3(QPointF const point) noexcept
  : m_x(static_cast<float>(point.x()))
  , m_y(static_cast<float>(point.y()))
{}

Point3::Point3(QVector2D const& vector) noexcept
  : m_x(vector.x())
  , m_y(vector.y())
{}

Point3::Point3(QVector3D const& vector) noexcept
  : m_x(vector.x())
  , m_y(vector.y())
  , m_z(vector.z())
{}

QPointF Point3::to_qpointf() const noexcept { return {this->m_x, this->m_y}; }

QVector2D Point3::to_qvector2d() const noexcept { return {this->m_x, this->m_y}; }

QVector3D Point3::to_qvector3d() const noexcept { return {this->m_x, this->m_y, this->m_z}; }

QString Point3::to_qstring() const { return QString::fromStdString(this->to_string()); }

Point3 Point3::from_qpointf(QPointF const point) noexcept { return Point3(point); }

Point3 Point3::from_qvector2d(QVector2D const& vector) noexcept { return Point3(vector); }

Point3 Point3::from_qvector3d(QVector3D const& vector) noexcept { return Point3(vector); }
#endif

std::ostream& operator<<(std::ostream& os, Point3 const point) { return os << point.to_string(); }

RADAR_COMMON_NAMESPACE_END
