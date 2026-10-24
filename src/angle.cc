#include <whs31/common/angle.h>

#include <fmt/format.h>

WHS31_COMMON_NAMESPACE_BEGIN

std::string Angle::to_string() const { return fmt::format("{:.6g} deg", this->degrees()); }

WHS31_COMMON_NAMESPACE_END
