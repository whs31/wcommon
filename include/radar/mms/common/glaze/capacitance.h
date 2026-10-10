#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/capacitance.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct CapacitanceFields
  {
    std::optional<double> farads;
    std::optional<double> microfarads;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Capacitance: `{"farads": magnitude}`.
 * @details Reading also accepts the magnitude in `microfarads` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Capacitance>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Capacitance& value,
      radar::mms::common::_glaze_impl::CapacitanceFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Capacitance>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {fields.farads, &radar::mms::common::Capacitance::from_farads},
        Unit {fields.microfarads, &radar::mms::common::Capacitance::from_microfarads}
      );
    },
    [](radar::mms::common::Capacitance const& value) {
      return radar::mms::common::_glaze_impl::CapacitanceFields {
        .farads = static_cast<double>(value)
      };
    }>;
};
