#pragma once

#include <whs31/common/euler_angles.h>
#include <whs31/common/glaze/angle.h>
#include <whs31/common/glaze/detail.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct EulerAnglesFields
  {
    Angle roll;
    Angle pitch;
    Angle yaw;
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of EulerAngles: `{"roll": angle, "pitch": angle, "yaw": angle}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::EulerAngles>
{
  static auto constexpr value = glz::custom<
    [](
      whs31::common::EulerAngles& angles,
      whs31::common::_glaze_impl::EulerAnglesFields const& fields
    ) { angles = whs31::common::EulerAngles(fields.roll, fields.pitch, fields.yaw); },
    [](whs31::common::EulerAngles const& angles) {
      return whs31::common::_glaze_impl::EulerAnglesFields {
        .roll = angles.roll(),
        .pitch = angles.pitch(),
        .yaw = angles.yaw(),
      };
    }>;
};
