#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/density.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct DensityFields
  {
    std::optional<double> kilograms_per_cubic_meter;
  };

  inline constexpr auto density_reading = QuantityReading<Density, DensityFields, 1, 1> {
    .fields =
      {
        field_unit(
          &DensityFields::kilograms_per_cubic_meter,
          &Density::from_kilograms_per_cubic_meter
        ),
      },
    .literals =
      {
        literal_unit("kg_per_m3", &Density::from_kilograms_per_cubic_meter),
      },
    .unknown_suffix = "a density literal needs one of the unit suffixes kg_per_m3",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Density: `{"kilograms_per_cubic_meter": magnitude}`.
 * @details A text format also reads a literal string, e.g. `"12.5kg_per_m3"`, with the suffix
 * of a C++ literal: `kg_per_m3`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Density>
{
  static constexpr auto value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::density_reading>::value;
};
