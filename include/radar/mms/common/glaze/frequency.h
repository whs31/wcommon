#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/frequency.h>

/**
 * @brief Glaze metadata of Frequency: `{"hertz": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Frequency>
{
  static constexpr auto value = glz::
    object("hertz", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Frequency>);
};
