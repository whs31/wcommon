#pragma once

#include <optional>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/magnetic_flux_density.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct MagneticFluxDensityFields
  {
    std::optional<double> teslas;
    std::optional<double> gauss;
    std::optional<double> microteslas;
    std::optional<double> nanoteslas;
  };

  inline auto constexpr magnetic_flux_density_reading = QuantityReading<
    MagneticFluxDensity,
    MagneticFluxDensityFields,
    4,
    3> {
    .fields =
      {
        field_unit(&MagneticFluxDensityFields::teslas, &MagneticFluxDensity::from_teslas),
        field_unit(&MagneticFluxDensityFields::gauss, &MagneticFluxDensity::from_gauss),
        field_unit(&MagneticFluxDensityFields::microteslas, &MagneticFluxDensity::from_microteslas),
        field_unit(&MagneticFluxDensityFields::nanoteslas, &MagneticFluxDensity::from_nanoteslas),
      },
    .literals =
      {
        literal_unit("gauss", &MagneticFluxDensity::from_gauss),
        literal_unit("ut", &MagneticFluxDensity::from_microteslas),
        literal_unit("nt", &MagneticFluxDensity::from_nanoteslas),
      },
    .unknown_suffix =
      "a magnetic flux density literal needs one of the unit suffixes gauss, ut, "
      "nt",
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of MagneticFluxDensity: `{"teslas": magnitude}`.
 * @details Reading also accepts the magnitude in `gauss`, `microteslas` or `nanoteslas`
 * instead. A text format also reads a literal string, e.g. `"12.5gauss"`, with the suffix of a
 * C++ literal: `gauss`, `ut`, `nt`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::MagneticFluxDensity>
{
  static auto constexpr value = whs31::common::_glaze_impl::QuantityAdapter<
    whs31::common::_glaze_impl::magnetic_flux_density_reading>::value;
};
