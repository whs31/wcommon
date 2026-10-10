#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/voltage.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct VoltageFields
  {
    std::optional<double> volts;
    std::optional<double> millivolts;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Voltage: `{"volts": magnitude}`.
 * @details Reading also accepts the magnitude in `millivolts` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Voltage>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Voltage& value,
      radar::mms::common::_glaze_impl::VoltageFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Voltage>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {fields.volts, &radar::mms::common::Voltage::from_volts},
        Unit {fields.millivolts, &radar::mms::common::Voltage::from_millivolts}
      );
    },
    [](radar::mms::common::Voltage const& value) {
      return radar::mms::common::_glaze_impl::VoltageFields {.volts = static_cast<double>(value)};
    }>;
};
