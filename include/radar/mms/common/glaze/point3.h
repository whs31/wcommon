#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/point3.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct Point3Fields
  {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Point3: `{"x": x, "y": y, "z": z}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Point3>
{
  static auto constexpr value = glz::custom<
    [](
      radar::mms::common::Point3& point3,
      radar::mms::common::_glaze_impl::Point3Fields const& fields
    ) { point3 = radar::mms::common::Point3::from_xyz(fields.x, fields.y, fields.z); },
    [](radar::mms::common::Point3 const& point3) {
      return radar::mms::common::_glaze_impl::Point3Fields {
        .x = point3.x(),
        .y = point3.y(),
        .z = point3.z(),
      };
    }>;
};
