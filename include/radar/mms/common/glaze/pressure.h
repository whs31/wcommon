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
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Pressure: `{"pascals": magnitude}`.
 * @details Reading also accepts the magnitude in `hectopascals`, `kilopascals`, `millibars` or
 * `inches_of_mercury` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Pressure>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Pressure& value,
      radar::mms::common::_glaze_impl::PressureFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Pressure>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.pascals,
          .from = &radar::mms::common::Pressure::from_pascals,
        },
        Unit {
          .magnitude = fields.hectopascals,
          .from = &radar::mms::common::Pressure::from_hectopascals,
        },
        Unit {
          .magnitude = fields.kilopascals,
          .from = &radar::mms::common::Pressure::from_kilopascals,
        },
        Unit {
          .magnitude = fields.millibars,
          .from = &radar::mms::common::Pressure::from_millibars,
        },
        Unit {
          .magnitude = fields.inches_of_mercury,
          .from = &radar::mms::common::Pressure::from_inches_of_mercury,
        }
      );
    },
    [](radar::mms::common::Pressure const& value) {
      return radar::mms::common::_glaze_impl::PressureFields {
        .pascals = static_cast<double>(value),
      };
    }>;
};
