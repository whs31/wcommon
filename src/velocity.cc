#include <radar/mms/common/velocity.h>

#include <ostream>

#include <fmt/format.h>

#ifdef RCOMMON_WITH_QT
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

std::string Velocity::to_string() const { return fmt::format("{:.6g} m/s", this->m_mps); }

#ifdef RCOMMON_WITH_QT
QString Velocity::to_qstring() const { return QString::fromStdString(this->to_string()); }
#endif

std::ostream& operator<<(std::ostream& os, Velocity const velocity)
{
  return os << velocity.to_string();
}

RADAR_COMMON_NAMESPACE_END
