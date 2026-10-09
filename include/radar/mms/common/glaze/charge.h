#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/charge.h>

/**
 * @brief Glaze metadata of Charge: `{"coulombs": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Charge>
{
  static constexpr auto value = glz::
    object("coulombs", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Charge>);
};
