#include <radar/mms/common/angle.h>

#include <fmt/format.h>

#ifdef RCOMMON_WITH_QT
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

std::string Angle::to_string() const { return fmt::format("{:.6g} deg", this->degrees()); }

#ifdef RCOMMON_WITH_QT
QString Angle::to_qstring() const { return QString::fromStdString(this->to_string()); }
#endif

std::ostream& operator<<(std::ostream& os, Angle const angle) { return os << angle.to_string(); }

RADAR_COMMON_NAMESPACE_END
