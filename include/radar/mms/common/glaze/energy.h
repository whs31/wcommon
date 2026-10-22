#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/energy.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct EnergyFields
  {
    std::optional<double> joules;
    std::optional<double> watt_hours;
    std::optional<double> kilowatt_hours;
  };

  inline auto constexpr energy_reading = QuantityReading<Energy, EnergyFields, 3, 3> {
    .fields =
      {
        field_unit(&EnergyFields::joules, &Energy::from_joules),
        field_unit(&EnergyFields::watt_hours, &Energy::from_watt_hours),
        field_unit(&EnergyFields::kilowatt_hours, &Energy::from_kilowatt_hours),
      },
    .literals =
      {
        literal_unit("j", &Energy::from_joules),
        literal_unit("wh", &Energy::from_watt_hours),
        literal_unit("kwh", &Energy::from_kilowatt_hours),
      },
    .unknown_suffix = "a energy literal needs one of the unit suffixes j, wh, kwh",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Energy: `{"joules": magnitude}`.
 * @details Reading also accepts the magnitude in `watt_hours` or `kilowatt_hours` instead. A
 * text format also reads a literal string, e.g. `"12.5j"`, with the suffix of a C++ literal:
 * `j`, `wh`, `kwh`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Energy>
{
  static auto constexpr value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::energy_reading>::value;
};
