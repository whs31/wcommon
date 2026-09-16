#include <radar/mms/common/vector2.h>

#include <ostream>

#include <fmt/format.h>

RADAR_COMMON_NAMESPACE_BEGIN

std::string Vector2::to_string() const { return fmt::format("({}, {})", this->m_x, this->m_y); }

#ifdef RCOMMON_WITH_QT
Vector2::Vector2(QPointF const point) noexcept
  : m_x(static_cast<float>(point.x()))
  , m_y(static_cast<float>(point.y()))
{}

Vector2::Vector2(QVector2D const& vector) noexcept
  : m_x(vector.x())
  , m_y(vector.y())
{}

QPointF Vector2::to_qpointf() const noexcept { return {this->m_x, this->m_y}; }

QVector2D Vector2::to_qvector2d() const noexcept { return {this->m_x, this->m_y}; }

QString Vector2::to_qstring() const { return QString::fromStdString(this->to_string()); }

Vector2 Vector2::from_qpointf(QPointF const point) noexcept { return Vector2(point); }

Vector2 Vector2::from_qvector2d(QVector2D const& vector) noexcept { return Vector2(vector); }
#endif

std::ostream& operator<<(std::ostream& os, Vector2 const vector)
{
  return os << vector.to_string();
}

RADAR_COMMON_NAMESPACE_END
