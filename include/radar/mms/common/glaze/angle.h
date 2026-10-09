#pragma once

#include <string>

#include <radar/mms/common/angle.h>
#include <radar/mms/common/glaze/detail.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct AngleFields
  {
    double angle = 0.0;
    std::string units;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Angle: `{"angle": degrees, "units": "degrees"}`.
 * @details Reading also accepts `"units": "radians"`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Angle>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Angle& angle,
      radar::mms::common::_glaze_impl::AngleFields const& fields,
      glz::context& ctx
    ) {
      if(fields.units == "degrees")
        angle = radar::mms::common::Angle::from_degrees(fields.angle);
      else if(fields.units == "radians")
        angle = radar::mms::common::Angle::from_radians(fields.angle);
      else
        radar::mms::common::_glaze_impl::
          reject(ctx, "invalid angle units: expected either degrees or radians");
    },
    [](radar::mms::common::Angle const& angle) {
      return radar::mms::common::_glaze_impl::AngleFields {
        .angle = angle.degrees(),
        .units = "degrees",
      };
    }>;
};
