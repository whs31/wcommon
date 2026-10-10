#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/velocity.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct VelocityFields
  {
    std::optional<double> mps;
    std::optional<double> kmph;
    std::optional<double> mph;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Velocity: `{"mps": magnitude}`.
 * @details Reading also accepts the magnitude in `kmph` or `mph` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Velocity>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Velocity& value,
      radar::mms::common::_glaze_impl::VelocityFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Velocity>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.mps,
          .from = &radar::mms::common::Velocity::from_mps,
        },
        Unit {
          .magnitude = fields.kmph,
          .from = &radar::mms::common::Velocity::from_kmph,
        },
        Unit {
          .magnitude = fields.mph,
          .from = &radar::mms::common::Velocity::from_mph,
        }
      );
    },
    [](radar::mms::common::Velocity const& value) {
      return radar::mms::common::_glaze_impl::VelocityFields {
        .mps = static_cast<double>(value),
      };
    }>;
};
