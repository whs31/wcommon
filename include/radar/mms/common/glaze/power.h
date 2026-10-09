#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/power.h>

/**
 * @brief Glaze metadata of Power: `{"watts": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Power>
{
  static constexpr auto value = glz::
    object("watts", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Power>);
};
