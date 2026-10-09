#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/acceleration.h>

/**
 * @brief Glaze metadata of Acceleration: `{"meters_per_second_squared": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Acceleration>
{
  static constexpr auto value = glz::object(
    "meters_per_second_squared",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Acceleration>
  );
};
