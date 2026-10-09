#include <radar/mms/common/euler_angles.h>
#include <radar/mms/common/quaternion.h>

#include <ostream>

#include <fmt/format.h>

#ifdef RCOMMON_WITH_QT
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

EulerAngles EulerAngles::from_quaternion(Quaternion const& attitude) noexcept
{
  return attitude.to_euler_angles();
}

Quaternion EulerAngles::to_quaternion() const noexcept
{
  return Quaternion::from_euler_angles(*this);
}

std::string EulerAngles::to_string() const
{
  return fmt::format("roll {}, pitch {}, yaw {}", this->m_roll, this->m_pitch, this->m_yaw);
}

#ifdef RCOMMON_WITH_QT
QString EulerAngles::to_qstring() const { return QString::fromStdString(this->to_string()); }
#endif

std::ostream& operator<<(std::ostream& os, EulerAngles const& angles)
{
  return os << angles.to_string();
}

RADAR_COMMON_NAMESPACE_END
