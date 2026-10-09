#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/angular_velocity.h>

/**
 * @brief Glaze metadata of AngularVelocity: `{"radians_per_second": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::AngularVelocity>
{
  static constexpr auto value = glz::object(
    "radians_per_second",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::AngularVelocity>
  );
};
