#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/power.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct PowerFields
  {
    std::optional<double> watts;
    std::optional<double> kilowatts;
  };

  inline auto constexpr power_reading = QuantityReading<Power, PowerFields, 2, 2> {
    .fields =
      {
        field_unit(&PowerFields::watts, &Power::from_watts),
        field_unit(&PowerFields::kilowatts, &Power::from_kilowatts),
      },
    .literals =
      {
        literal_unit("w", &Power::from_watts),
        literal_unit("kw", &Power::from_kilowatts),
      },
    .unknown_suffix = "a power literal needs one of the unit suffixes w, kw",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Power: `{"watts": magnitude}`.
 * @details Reading also accepts the magnitude in `kilowatts` instead. A text format also reads
 * a literal string, e.g. `"12.5w"`, with the suffix of a C++ literal: `w`, `kw`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Power>
{
  static auto constexpr value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::power_reading>::value;
};
