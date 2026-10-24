#pragma once

#include <whs31/common/glaze/detail.h>
#include <whs31/common/point3.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct Point3Fields
  {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Point3: `{"x": x, "y": y, "z": z}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Point3>
{
  static auto constexpr value = glz::custom<
    [](whs31::common::Point3& point3, whs31::common::_glaze_impl::Point3Fields const& fields) {
      point3 = whs31::common::Point3::from_xyz(fields.x, fields.y, fields.z);
    },
    [](whs31::common::Point3 const& point3) {
      return whs31::common::_glaze_impl::Point3Fields {
        .x = point3.x(),
        .y = point3.y(),
        .z = point3.z(),
      };
    }>;
};
