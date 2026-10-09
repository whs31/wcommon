#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/angular_acceleration.h>

/**
 * @brief Glaze metadata of AngularAcceleration: `{"radians_per_second_squared": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::AngularAcceleration>
{
  static constexpr auto value = glz::object(
    "radians_per_second_squared",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::AngularAcceleration>
  );
};
