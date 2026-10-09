#include <radar/mms/common/angle.h>

#include <fmt/format.h>

RADAR_COMMON_NAMESPACE_BEGIN

std::string Angle::to_string() const { return fmt::format("{:.6g} deg", this->degrees()); }

RADAR_COMMON_NAMESPACE_END
