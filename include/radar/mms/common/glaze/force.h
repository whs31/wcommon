#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/force.h>

/**
 * @brief Glaze metadata of Force: `{"newtons": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Force>
{
  static constexpr auto value = glz::
    object("newtons", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Force>);
};
