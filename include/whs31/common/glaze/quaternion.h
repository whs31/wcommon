#pragma once

#include <whs31/common/glaze/detail.h>
#include <whs31/common/quaternion.h>

WHS31_COMMON_NAMESPACE_BEGIN

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

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Quaternion: `{"w": w, "x": x, "y": y, "z": z}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Quaternion>
{
  static auto constexpr value = glz::custom<
    [](
      whs31::common::Quaternion& quaternion,
      whs31::common::_glaze_impl::QuaternionFields const& fields
    ) {
      quaternion = whs31::common::Quaternion::from_wxyz(fields.w, fields.x, fields.y, fields.z);
    },
    [](whs31::common::Quaternion const& quaternion) {
      return whs31::common::_glaze_impl::QuaternionFields {
        .w = quaternion.w(),
        .x = quaternion.x(),
        .y = quaternion.y(),
        .z = quaternion.z(),
      };
    }>;
};
