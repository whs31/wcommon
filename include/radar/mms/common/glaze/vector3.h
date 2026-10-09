#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/vector3.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct Vector3Fields
  {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Vector3: `{"x": x, "y": y, "z": z}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Vector3>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Vector3& vector3,
      radar::mms::common::_glaze_impl::Vector3Fields const& fields
    ) { vector3 = radar::mms::common::Vector3::from_xyz(fields.x, fields.y, fields.z); },
    [](radar::mms::common::Vector3 const& vector3) {
      return radar::mms::common::_glaze_impl::Vector3Fields {
        .x = vector3.x(),
        .y = vector3.y(),
        .z = vector3.z(),
      };
    }>;
};
