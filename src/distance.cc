#include <radar/mms/common/distance.h>

#include <cmath>
#include <ostream>

#include <fmt/format.h>

#ifdef RCOMMON_WITH_QT
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

std::string Distance::to_string() const
{
  auto const magnitude = std::abs(this->m_meters);
  if(magnitude < 0.1F)
    return fmt::format("{:.6g} mm", this->as_mm());
  if(magnitude < 1.0F)
    return fmt::format("{:.6g} cm", this->as_cm());
  if(magnitude < 1'000.0F)
    return fmt::format("{:.6g} m", this->as_m());
  return fmt::format("{:.6g} km", this->as_km());
}

#ifdef RCOMMON_WITH_QT
QString Distance::to_qstring() const { return QString::fromStdString(this->to_string()); }
#endif

std::ostream& operator<<(std::ostream& os, Distance const distance)
{
  return os << distance.to_string();
}

RADAR_COMMON_NAMESPACE_END
