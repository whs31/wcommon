#include <radar/mms/common/distance.h>

#include <cmath>

#include <fmt/format.h>

RADAR_COMMON_NAMESPACE_BEGIN

std::string Distance::to_string() const
{
  auto const magnitude = std::abs(this->magnitude());
  if(magnitude < 0.1)
    return fmt::format("{:.6g} mm", this->as_mm());
  if(magnitude < 1.0)
    return fmt::format("{:.6g} cm", this->as_cm());
  if(magnitude < 1'000.0)
    return fmt::format("{:.6g} m", this->as_m());
  return fmt::format("{:.6g} km", this->as_km());
}

RADAR_COMMON_NAMESPACE_END
