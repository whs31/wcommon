#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/magnetic_flux_density.h>

/**
 * @brief Glaze metadata of MagneticFluxDensity: `{"teslas": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::MagneticFluxDensity>
{
  static constexpr auto value = glz::object(
    "teslas",
    radar::mms::common::_glaze_impl::si_magnitude<radar::mms::common::MagneticFluxDensity>
  );
};
