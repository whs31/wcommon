#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/angular_velocity.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct AngularVelocityFields
  {
    std::optional<double> radians_per_second;
    std::optional<double> degrees_per_second;
    std::optional<double> rpm;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of AngularVelocity: `{"radians_per_second": magnitude}`.
 * @details Reading also accepts the magnitude in `degrees_per_second` or `rpm` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::AngularVelocity>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::AngularVelocity& value,
      radar::mms::common::_glaze_impl::AngularVelocityFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<
        radar::mms::common::AngularVelocity>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.radians_per_second,
          .from = &radar::mms::common::AngularVelocity::from_radians_per_second,
        },
        Unit {
          .magnitude = fields.degrees_per_second,
          .from = &radar::mms::common::AngularVelocity::from_degrees_per_second,
        },
        Unit {
          .magnitude = fields.rpm,
          .from = &radar::mms::common::AngularVelocity::from_rpm,
        }
      );
    },
    [](radar::mms::common::AngularVelocity const& value) {
      return radar::mms::common::_glaze_impl::AngularVelocityFields {
        .radians_per_second = static_cast<double>(value),
      };
    }>;
};
