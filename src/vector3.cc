#include <radar/mms/common/vector3.h>

#include <ostream>

#include <fmt/format.h>

RADAR_COMMON_NAMESPACE_BEGIN

std::string Vector3::to_string() const { return fmt::format("{}", *this); }

#ifdef RCOMMON_WITH_QT
Vector3::Vector3(QPointF const point) noexcept
  : m_x(static_cast<double>(point.x()))
  , m_y(static_cast<double>(point.y()))
{}

Vector3::Vector3(QVector2D const& vector) noexcept
  : m_x(vector.x())
  , m_y(vector.y())
{}

Vector3::Vector3(QVector3D const& vector) noexcept
  : m_x(vector.x())
  , m_y(vector.y())
  , m_z(vector.z())
{}

QPointF Vector3::to_qpointf() const noexcept { return {this->m_x, this->m_y}; }

QVector2D Vector3::to_qvector2d() const noexcept
{
  return {static_cast<float>(this->m_x), static_cast<float>(this->m_y)};
}

QVector3D Vector3::to_qvector3d() const noexcept
{
  return {
    static_cast<float>(this->m_x),
    static_cast<float>(this->m_y),
    static_cast<float>(this->m_z)
  };
}

QString Vector3::to_qstring() const { return QString::fromStdString(this->to_string()); }

Vector3 Vector3::from_qpointf(QPointF const point) noexcept { return Vector3(point); }

Vector3 Vector3::from_qvector2d(QVector2D const& vector) noexcept { return Vector3(vector); }

Vector3 Vector3::from_qvector3d(QVector3D const& vector) noexcept { return Vector3(vector); }
#endif

std::ostream& operator<<(std::ostream& os, Vector3 const vector)
{
  return os << vector.to_string();
}

RADAR_COMMON_NAMESPACE_END
