#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/conductance.h>

/**
 * @brief Glaze metadata of Conductance: `{"siemens": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Conductance>
{
  static constexpr auto value = glz::object(
    "siemens",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Conductance>
  );
};
