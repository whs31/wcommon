#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/temperature.h>

/**
 * @brief Glaze metadata of Temperature: `{"kelvin": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Temperature>
{
  static constexpr auto value = glz::object(
    "kelvin",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Temperature>
  );
};
