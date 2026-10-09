#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/density.h>

/**
 * @brief Glaze metadata of Density: `{"kilograms_per_cubic_meter": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Density>
{
  static constexpr auto value = glz::object(
    "kilograms_per_cubic_meter",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Density>
  );
};
