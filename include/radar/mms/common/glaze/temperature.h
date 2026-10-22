#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/temperature.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct TemperatureFields
  {
    std::optional<double> kelvin;
    std::optional<double> celsius;
    std::optional<double> fahrenheit;
  };

  inline auto constexpr temperature_reading = QuantityReading<
    Temperature,
    TemperatureFields,
    3,
    3> {
    .fields =
      {
        field_unit(&TemperatureFields::kelvin, &Temperature::from_kelvin),
        field_unit(&TemperatureFields::celsius, &Temperature::from_celsius),
        field_unit(&TemperatureFields::fahrenheit, &Temperature::from_fahrenheit),
      },
    .literals =
      {
        literal_unit("k", &Temperature::from_kelvin),
        literal_unit("degc", &Temperature::from_celsius),
        literal_unit("degf", &Temperature::from_fahrenheit),
      },
    .unknown_suffix = "a temperature literal needs one of the unit suffixes k, degc, degf",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Temperature: `{"kelvin": magnitude}`.
 * @details Reading also accepts the magnitude in `celsius` or `fahrenheit` instead. A text
 * format also reads a literal string, e.g. `"12.5k"`, with the suffix of a C++ literal: `k`,
 * `degc`, `degf`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Temperature>
{
  static auto constexpr value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::temperature_reading>::value;
};
