#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/pressure.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct PressureFields
  {
    std::optional<double> pascals;
    std::optional<double> hectopascals;
    std::optional<double> kilopascals;
    std::optional<double> millibars;
    std::optional<double> inches_of_mercury;
  };

  inline constexpr auto pressure_reading = QuantityReading<Pressure, PressureFields, 5, 5> {
    .fields =
      {
        field_unit(&PressureFields::pascals, &Pressure::from_pascals),
        field_unit(&PressureFields::hectopascals, &Pressure::from_hectopascals),
        field_unit(&PressureFields::kilopascals, &Pressure::from_kilopascals),
        field_unit(&PressureFields::millibars, &Pressure::from_millibars),
        field_unit(&PressureFields::inches_of_mercury, &Pressure::from_inches_of_mercury),
      },
    .literals =
      {
        literal_unit("pa", &Pressure::from_pascals),
        literal_unit("hpa", &Pressure::from_hectopascals),
        literal_unit("kpa", &Pressure::from_kilopascals),
        literal_unit("mbar", &Pressure::from_millibars),
        literal_unit("inhg", &Pressure::from_inches_of_mercury),
      },
    .unknown_suffix = "a pressure literal needs one of the unit suffixes pa, hpa, kpa, mbar, inhg",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Pressure: `{"pascals": magnitude}`.
 * @details Reading also accepts the magnitude in `hectopascals`, `kilopascals`, `millibars` or
 * `inches_of_mercury` instead. A text format also reads a literal string, e.g. `"12.5pa"`, with
 * the suffix of a C++ literal: `pa`, `hpa`, `kpa`, `mbar`, `inhg`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Pressure>
{
  static constexpr auto value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::pressure_reading>::value;
};
