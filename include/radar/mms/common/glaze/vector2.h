#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/vector2.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct Vector2Fields
  {
    double x = 0.0;
    double y = 0.0;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Vector2: `{"x": x, "y": y}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Vector2>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Vector2& vector2,
      radar::mms::common::_glaze_impl::Vector2Fields const& fields
    ) { vector2 = radar::mms::common::Vector2::from_xy(fields.x, fields.y); },
    [](radar::mms::common::Vector2 const& vector2) {
      return radar::mms::common::_glaze_impl::Vector2Fields {
        .x = vector2.x(),
        .y = vector2.y(),
      };
    }>;
};
