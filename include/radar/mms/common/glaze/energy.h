#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/energy.h>

/**
 * @brief Glaze metadata of Energy: `{"joules": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Energy>
{
  static constexpr auto value = glz::
    object("joules", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Energy>);
};
