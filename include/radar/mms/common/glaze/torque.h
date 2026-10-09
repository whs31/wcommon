#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/torque.h>

/**
 * @brief Glaze metadata of Torque: `{"newton_meters": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Torque>
{
  static constexpr auto value = glz::object(
    "newton_meters",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Torque>
  );
};
