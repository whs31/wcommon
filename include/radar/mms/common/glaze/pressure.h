#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/pressure.h>

/**
 * @brief Glaze metadata of Pressure: `{"pascals": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Pressure>
{
  static constexpr auto value = glz::
    object("pascals", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Pressure>);
};
