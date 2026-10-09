#pragma once

#include <radar/mms/common/euler_angles.h>
#include <radar/mms/common/glaze/angle.h>
#include <radar/mms/common/glaze/detail.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct EulerAnglesFields
  {
    Angle roll;
    Angle pitch;
    Angle yaw;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of EulerAngles: `{"roll": angle, "pitch": angle, "yaw": angle}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::EulerAngles>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::EulerAngles& angles,
      radar::mms::common::_glaze_impl::EulerAnglesFields const& fields
    ) { angles = radar::mms::common::EulerAngles(fields.roll, fields.pitch, fields.yaw); },
    [](radar::mms::common::EulerAngles const& angles) {
      return radar::mms::common::_glaze_impl::EulerAnglesFields {
        .roll = angles.roll(),
        .pitch = angles.pitch(),
        .yaw = angles.yaw(),
      };
    }>;
};
