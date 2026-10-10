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
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Energy: `{"joules": magnitude}`.
 * @details Reading also accepts the magnitude in `watt_hours` or `kilowatt_hours` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Energy>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Energy& value,
      radar::mms::common::_glaze_impl::EnergyFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Energy>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.joules,
          .from = &radar::mms::common::Energy::from_joules,
        },
        Unit {
          .magnitude = fields.watt_hours,
          .from = &radar::mms::common::Energy::from_watt_hours,
        },
        Unit {
          .magnitude = fields.kilowatt_hours,
          .from = &radar::mms::common::Energy::from_kilowatt_hours,
        }
      );
    },
    [](radar::mms::common::Energy const& value) {
      return radar::mms::common::_glaze_impl::EnergyFields {
        .joules = static_cast<double>(value),
      };
    }>;
};
