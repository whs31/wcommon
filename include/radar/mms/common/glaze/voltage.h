#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/voltage.h>

/**
 * @brief Glaze metadata of Voltage: `{"volts": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Voltage>
{
  static constexpr auto value = glz::
    object("volts", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Voltage>);
};
