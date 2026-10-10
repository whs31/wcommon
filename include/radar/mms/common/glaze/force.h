#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/force.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct ForceFields
  {
    std::optional<double> newtons;
    std::optional<double> kilonewtons;
    std::optional<double> kilograms_force;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Force: `{"newtons": magnitude}`.
 * @details Reading also accepts the magnitude in `kilonewtons` or `kilograms_force` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Force>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Force& value,
      radar::mms::common::_glaze_impl::ForceFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Force>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.newtons,
          .from = &radar::mms::common::Force::from_newtons,
        },
        Unit {
          .magnitude = fields.kilonewtons,
          .from = &radar::mms::common::Force::from_kilonewtons,
        },
        Unit {
          .magnitude = fields.kilograms_force,
          .from = &radar::mms::common::Force::from_kilograms_force,
        }
      );
    },
    [](radar::mms::common::Force const& value) {
      return radar::mms::common::_glaze_impl::ForceFields {
        .newtons = static_cast<double>(value),
      };
    }>;
};
