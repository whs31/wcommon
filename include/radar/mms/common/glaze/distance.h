#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/distance.h>

/**
 * @brief Glaze metadata of Distance: `{"meters": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Distance>
{
  static constexpr auto value = glz::
    object("meters", radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Distance>);
};
