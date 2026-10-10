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
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Frequency: `{"hertz": magnitude}`.
 * @details Reading also accepts the magnitude in `kilohertz` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Frequency>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Frequency& value,
      radar::mms::common::_glaze_impl::FrequencyFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Frequency>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.hertz,
          .from = &radar::mms::common::Frequency::from_hertz,
        },
        Unit {
          .magnitude = fields.kilohertz,
          .from = &radar::mms::common::Frequency::from_kilohertz,
        }
      );
    },
    [](radar::mms::common::Frequency const& value) {
      return radar::mms::common::_glaze_impl::FrequencyFields {
        .hertz = static_cast<double>(value),
      };
    }>;
};
