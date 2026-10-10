#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/current.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct CurrentFields
  {
    std::optional<double> amperes;
    std::optional<double> milliamperes;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Current: `{"amperes": magnitude}`.
 * @details Reading also accepts the magnitude in `milliamperes` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Current>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Current& value,
      radar::mms::common::_glaze_impl::CurrentFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Current>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {fields.amperes, &radar::mms::common::Current::from_amperes},
        Unit {fields.milliamperes, &radar::mms::common::Current::from_milliamperes}
      );
    },
    [](radar::mms::common::Current const& value) {
      return radar::mms::common::_glaze_impl::CurrentFields {.amperes = static_cast<double>(value)};
    }>;
};
