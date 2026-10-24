#pragma once

#include <optional>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/distance.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct DistanceFields
  {
    std::optional<double> meters;
    std::optional<double> nanometers;
    std::optional<double> micrometers;
    std::optional<double> millimeters;
    std::optional<double> centimeters;
    std::optional<double> decimeters;
    std::optional<double> hectometers;
    std::optional<double> kilometers;
    std::optional<double> inches;
    std::optional<double> feet;
    std::optional<double> yards;
    std::optional<double> furlongs;
    std::optional<double> miles;
  };

  inline auto constexpr distance_reading = QuantityReading<Distance, DistanceFields, 13, 6> {
    .fields =
      {
        field_unit(&DistanceFields::meters, &Distance::from_meters),
        field_unit(&DistanceFields::nanometers, &Distance::from_nanometers),
        field_unit(&DistanceFields::micrometers, &Distance::from_micrometers),
        field_unit(&DistanceFields::millimeters, &Distance::from_millimeters),
        field_unit(&DistanceFields::centimeters, &Distance::from_centimeters),
        field_unit(&DistanceFields::decimeters, &Distance::from_decimeters),
        field_unit(&DistanceFields::hectometers, &Distance::from_hectometers),
        field_unit(&DistanceFields::kilometers, &Distance::from_kilometers),
        field_unit(&DistanceFields::inches, &Distance::from_inches),
        field_unit(&DistanceFields::feet, &Distance::from_feet),
        field_unit(&DistanceFields::yards, &Distance::from_yards),
        field_unit(&DistanceFields::furlongs, &Distance::from_furlongs),
        field_unit(&DistanceFields::miles, &Distance::from_miles),
      },
    .literals =
      {
        literal_unit("meters", &Distance::from_meters),
        literal_unit("kilometers", &Distance::from_kilometers),
        literal_unit("m", &Distance::from_meters),
        literal_unit("km", &Distance::from_kilometers),
        literal_unit("cm", &Distance::from_centimeters),
        literal_unit("mm", &Distance::from_millimeters),
      },
    .unknown_suffix =
      "a distance literal needs one of the unit suffixes meters, kilometers, m, km, cm, mm",
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Distance: `{"meters": magnitude}`.
 * @details Reading also accepts the magnitude in `nanometers`, `micrometers`, `millimeters`,
 * `centimeters`, `decimeters`, `hectometers`, `kilometers`, `inches`, `feet`, `yards`,
 * `furlongs` or `miles` instead. A text format also reads a literal string, e.g.
 * `"12.5meters"`, with the suffix of a C++ literal: `meters`, `kilometers`, `m`, `km`, `cm`,
 * `mm`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Distance>
{
  static auto constexpr value = whs31::common::_glaze_impl::QuantityAdapter<
    whs31::common::_glaze_impl::distance_reading>::value;
};
