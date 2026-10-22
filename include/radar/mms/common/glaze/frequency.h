#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/frequency.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct FrequencyFields
  {
    std::optional<double> hertz;
    std::optional<double> kilohertz;
  };

  inline auto constexpr frequency_reading = QuantityReading<Frequency, FrequencyFields, 2, 2> {
    .fields =
      {
        field_unit(&FrequencyFields::hertz, &Frequency::from_hertz),
        field_unit(&FrequencyFields::kilohertz, &Frequency::from_kilohertz),
      },
    .literals =
      {
        literal_unit("hz", &Frequency::from_hertz),
        literal_unit("khz", &Frequency::from_kilohertz),
      },
    .unknown_suffix = "a frequency literal needs one of the unit suffixes hz, khz",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Frequency: `{"hertz": magnitude}`.
 * @details Reading also accepts the magnitude in `kilohertz` instead. A text format also reads
 * a literal string, e.g. `"12.5hz"`, with the suffix of a C++ literal: `hz`, `khz`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Frequency>
{
  static auto constexpr value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::frequency_reading>::value;
};
