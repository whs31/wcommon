#pragma once

#include <whs31/common/glaze/detail.h>
#include <whs31/common/vector3.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct Vector3Fields
  {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Vector3: `{"x": x, "y": y, "z": z}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Vector3>
{
  static auto constexpr value = glz::custom<
    [](whs31::common::Vector3& vector3, whs31::common::_glaze_impl::Vector3Fields const& fields) {
      vector3 = whs31::common::Vector3::from_xyz(fields.x, fields.y, fields.z);
    },
    [](whs31::common::Vector3 const& vector3) {
      return whs31::common::_glaze_impl::Vector3Fields {
        .x = vector3.x(),
        .y = vector3.y(),
        .z = vector3.z(),
      };
    }>;
};
