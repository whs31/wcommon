#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/acceleration.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct AccelerationFields
  {
    std::optional<double> meters_per_second_squared;
    std::optional<double> standard_gravity;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Acceleration: `{"meters_per_second_squared": magnitude}`.
 * @details Reading also accepts the magnitude in `standard_gravity` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Acceleration>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Acceleration& value,
      radar::mms::common::_glaze_impl::AccelerationFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Acceleration>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {fields.meters_per_second_squared, &radar::mms::common::Acceleration::from_mps2},
        Unit {fields.standard_gravity, &radar::mms::common::Acceleration::from_standard_gravity}
      );
    },
    [](radar::mms::common::Acceleration const& value) {
      return radar::mms::common::_glaze_impl::AccelerationFields {
        .meters_per_second_squared = static_cast<double>(value)
      };
    }>;
};
