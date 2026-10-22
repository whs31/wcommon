#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/quaternion.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct QuaternionFields
  {
    double w = 0.0;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Quaternion: `{"w": w, "x": x, "y": y, "z": z}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Quaternion>
{
  static auto constexpr value = glz::custom<
    [](
      radar::mms::common::Quaternion& quaternion,
      radar::mms::common::_glaze_impl::QuaternionFields const& fields
    ) {
      quaternion = radar::mms::common::Quaternion::
        from_wxyz(fields.w, fields.x, fields.y, fields.z);
    },
    [](radar::mms::common::Quaternion const& quaternion) {
      return radar::mms::common::_glaze_impl::QuaternionFields {
        .w = quaternion.w(),
        .x = quaternion.x(),
        .y = quaternion.y(),
        .z = quaternion.z(),
      };
    }>;
};
