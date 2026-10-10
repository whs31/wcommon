#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/mass.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct MassFields
  {
    std::optional<double> kilograms;
    std::optional<double> grams;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Mass: `{"kilograms": magnitude}`.
 * @details Reading also accepts the magnitude in `grams` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Mass>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Mass& value,
      radar::mms::common::_glaze_impl::MassFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Mass>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.kilograms,
          .from = &radar::mms::common::Mass::from_kilograms,
        },
        Unit {
          .magnitude = fields.grams,
          .from = &radar::mms::common::Mass::from_grams,
        }
      );
    },
    [](radar::mms::common::Mass const& value) {
      return radar::mms::common::_glaze_impl::MassFields {
        .kilograms = static_cast<double>(value),
      };
    }>;
};
