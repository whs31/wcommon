#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/capacitance.h>

/**
 * @brief Glaze metadata of Capacitance: `{"farads": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Capacitance>
{
  static constexpr auto value = glz::object(
    "farads",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::Capacitance>
  );
};
