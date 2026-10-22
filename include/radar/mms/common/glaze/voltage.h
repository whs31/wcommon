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

  inline auto constexpr voltage_reading = QuantityReading<Voltage, VoltageFields, 2, 2> {
    .fields =
      {
        field_unit(&VoltageFields::volts, &Voltage::from_volts),
        field_unit(&VoltageFields::millivolts, &Voltage::from_millivolts),
      },
    .literals =
      {
        literal_unit("v", &Voltage::from_volts),
        literal_unit("mv", &Voltage::from_millivolts),
      },
    .unknown_suffix = "a voltage literal needs one of the unit suffixes v, mv",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Voltage: `{"volts": magnitude}`.
 * @details Reading also accepts the magnitude in `millivolts` instead. A text format also reads
 * a literal string, e.g. `"12.5v"`, with the suffix of a C++ literal: `v`, `mv`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Voltage>
{
  static auto constexpr value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::voltage_reading>::value;
};
