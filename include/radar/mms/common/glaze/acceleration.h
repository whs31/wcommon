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

  inline auto constexpr acceleration_reading = QuantityReading<
    Acceleration,
    AccelerationFields,
    2,
    2> {
    .fields =
      {
        field_unit(&AccelerationFields::meters_per_second_squared, &Acceleration::from_mps2),
        field_unit(&AccelerationFields::standard_gravity, &Acceleration::from_standard_gravity),
      },
    .literals =
      {
        literal_unit("mps2", &Acceleration::from_mps2),
        literal_unit("gn", &Acceleration::from_standard_gravity),
      },
    .unknown_suffix = "a acceleration literal needs one of the unit suffixes mps2, gn",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Acceleration: `{"meters_per_second_squared": magnitude}`.
 * @details Reading also accepts the magnitude in `standard_gravity` instead. A text format also
 * reads a literal string, e.g. `"12.5mps2"`, with the suffix of a C++ literal: `mps2`, `gn`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Acceleration>
{
  static auto constexpr value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::acceleration_reading>::value;
};
