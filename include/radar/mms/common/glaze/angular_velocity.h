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

  inline auto constexpr
    angular_velocity_reading = QuantityReading<AngularVelocity, AngularVelocityFields, 3, 3> {
      .fields =
        {
          field_unit(
            &AngularVelocityFields::radians_per_second,
            &AngularVelocity::from_radians_per_second
          ),
          field_unit(
            &AngularVelocityFields::degrees_per_second,
            &AngularVelocity::from_degrees_per_second
          ),
          field_unit(&AngularVelocityFields::rpm, &AngularVelocity::from_rpm),
        },
      .literals =
        {
          literal_unit("rad_per_s", &AngularVelocity::from_radians_per_second),
          literal_unit("deg_per_s", &AngularVelocity::from_degrees_per_second),
          literal_unit("rpm", &AngularVelocity::from_rpm),
        },
      .unknown_suffix =
        "a angular velocity literal needs one of the unit suffixes rad_per_s, deg_per_s, rpm",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of AngularVelocity: `{"radians_per_second": magnitude}`.
 * @details Reading also accepts the magnitude in `degrees_per_second` or `rpm` instead. A text
 * format also reads a literal string, e.g. `"12.5rad_per_s"`, with the suffix of a C++ literal:
 * `rad_per_s`, `deg_per_s`, `rpm`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::AngularVelocity>
{
  static auto constexpr value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::angular_velocity_reading>::value;
};
