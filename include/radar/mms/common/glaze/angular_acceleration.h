#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/angular_acceleration.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct AngularAccelerationFields
  {
    std::optional<double> radians_per_second_squared;
    std::optional<double> degrees_per_second_squared;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of AngularAcceleration: `{"radians_per_second_squared": magnitude}`.
 * @details Reading also accepts the magnitude in `degrees_per_second_squared` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::AngularAcceleration>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::AngularAcceleration& value,
      radar::mms::common::_glaze_impl::AngularAccelerationFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<
        radar::mms::common::AngularAcceleration>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.radians_per_second_squared,
          .from = &radar::mms::common::AngularAcceleration::from_radians_per_second_squared,
        },
        Unit {
          .magnitude = fields.degrees_per_second_squared,
          .from = &radar::mms::common::AngularAcceleration::from_degrees_per_second_squared,
        }
      );
    },
    [](radar::mms::common::AngularAcceleration const& value) {
      return radar::mms::common::_glaze_impl::AngularAccelerationFields {
        .radians_per_second_squared = static_cast<double>(value),
      };
    }>;
};
