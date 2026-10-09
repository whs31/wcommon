#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/moment_of_inertia.h>

/**
 * @brief Glaze metadata of MomentOfInertia: `{"kilogram_square_meters": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::MomentOfInertia>
{
  static constexpr auto value = glz::object(
    "kilogram_square_meters",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::MomentOfInertia>
  );
};
