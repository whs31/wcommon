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
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Power: `{"watts": magnitude}`.
 * @details Reading also accepts the magnitude in `kilowatts` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Power>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Power& value,
      radar::mms::common::_glaze_impl::PowerFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Power>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {fields.watts, &radar::mms::common::Power::from_watts},
        Unit {fields.kilowatts, &radar::mms::common::Power::from_kilowatts}
      );
    },
    [](radar::mms::common::Power const& value) {
      return radar::mms::common::_glaze_impl::PowerFields {.watts = static_cast<double>(value)};
    }>;
};
