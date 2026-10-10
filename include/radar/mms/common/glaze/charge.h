#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/charge.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct ChargeFields
  {
    std::optional<double> coulombs;
    std::optional<double> ampere_hours;
    std::optional<double> milliampere_hours;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Charge: `{"coulombs": magnitude}`.
 * @details Reading also accepts the magnitude in `ampere_hours` or `milliampere_hours` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Charge>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Charge& value,
      radar::mms::common::_glaze_impl::ChargeFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Charge>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.coulombs,
          .from = &radar::mms::common::Charge::from_coulombs,
        },
        Unit {
          .magnitude = fields.ampere_hours,
          .from = &radar::mms::common::Charge::from_ampere_hours,
        },
        Unit {
          .magnitude = fields.milliampere_hours,
          .from = &radar::mms::common::Charge::from_milliampere_hours,
        }
      );
    },
    [](radar::mms::common::Charge const& value) {
      return radar::mms::common::_glaze_impl::ChargeFields {
        .coulombs = static_cast<double>(value),
      };
    }>;
};
