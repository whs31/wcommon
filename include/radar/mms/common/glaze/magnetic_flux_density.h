#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/magnetic_flux_density.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct MagneticFluxDensityFields
  {
    std::optional<double> teslas;
    std::optional<double> gauss;
    std::optional<double> microteslas;
    std::optional<double> nanoteslas;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of MagneticFluxDensity: `{"teslas": magnitude}`.
 * @details Reading also accepts the magnitude in `gauss`, `microteslas` or `nanoteslas` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::MagneticFluxDensity>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::MagneticFluxDensity& value,
      radar::mms::common::_glaze_impl::MagneticFluxDensityFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<
        radar::mms::common::MagneticFluxDensity>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {fields.teslas, &radar::mms::common::MagneticFluxDensity::from_teslas},
        Unit {fields.gauss, &radar::mms::common::MagneticFluxDensity::from_gauss},
        Unit {fields.microteslas, &radar::mms::common::MagneticFluxDensity::from_microteslas},
        Unit {fields.nanoteslas, &radar::mms::common::MagneticFluxDensity::from_nanoteslas}
      );
    },
    [](radar::mms::common::MagneticFluxDensity const& value) {
      return radar::mms::common::_glaze_impl::MagneticFluxDensityFields {
        .teslas = static_cast<double>(value)
      };
    }>;
};
