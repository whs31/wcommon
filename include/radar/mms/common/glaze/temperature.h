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
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Temperature: `{"kelvin": magnitude}`.
 * @details Reading also accepts the magnitude in `celsius` or `fahrenheit` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Temperature>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Temperature& value,
      radar::mms::common::_glaze_impl::TemperatureFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Temperature>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.kelvin,
          .from = &radar::mms::common::Temperature::from_kelvin,
        },
        Unit {
          .magnitude = fields.celsius,
          .from = &radar::mms::common::Temperature::from_celsius,
        },
        Unit {
          .magnitude = fields.fahrenheit,
          .from = &radar::mms::common::Temperature::from_fahrenheit,
        }
      );
    },
    [](radar::mms::common::Temperature const& value) {
      return radar::mms::common::_glaze_impl::TemperatureFields {
        .kelvin = static_cast<double>(value),
      };
    }>;
};
