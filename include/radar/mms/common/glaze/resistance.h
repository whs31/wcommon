#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/resistance.h>

/**
 * @brief Glaze metadata of Resistance: `{"ohms": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Resistance>
{
  static constexpr auto value = glz::
    object("ohms", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Resistance>);
};
