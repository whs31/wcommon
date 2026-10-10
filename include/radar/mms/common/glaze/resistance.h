#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/resistance.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct ResistanceFields
  {
    std::optional<double> ohms;
    std::optional<double> milliohms;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Resistance: `{"ohms": magnitude}`.
 * @details Reading also accepts the magnitude in `milliohms` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Resistance>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Resistance& value,
      radar::mms::common::_glaze_impl::ResistanceFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Resistance>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.ohms,
          .from = &radar::mms::common::Resistance::from_ohms,
        },
        Unit {
          .magnitude = fields.milliohms,
          .from = &radar::mms::common::Resistance::from_milliohms,
        }
      );
    },
    [](radar::mms::common::Resistance const& value) {
      return radar::mms::common::_glaze_impl::ResistanceFields {
        .ohms = static_cast<double>(value),
      };
    }>;
};
