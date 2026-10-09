#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/velocity.h>

/**
 * @brief Glaze metadata of Velocity: `{"mps": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Velocity>
{
  static constexpr auto value = glz::
    object("mps", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Velocity>);
};
