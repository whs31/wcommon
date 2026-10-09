#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/mass.h>

/**
 * @brief Glaze metadata of Mass: `{"kilograms": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Mass>
{
  static constexpr auto value = glz::
    object("kilograms", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Mass>);
};
