#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/current.h>

/**
 * @brief Glaze metadata of Current: `{"amperes": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Current>
{
  static constexpr auto value = glz::
    object("amperes", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Current>);
};
