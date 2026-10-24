#pragma once

#include <whs31/common/glaze/detail.h>
#include <whs31/common/vector2.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct Vector2Fields
  {
    double x = 0.0;
    double y = 0.0;
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Vector2: `{"x": x, "y": y}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Vector2>
{
  static auto constexpr value = glz::custom<
    [](whs31::common::Vector2& vector2, whs31::common::_glaze_impl::Vector2Fields const& fields) {
      vector2 = whs31::common::Vector2::from_xy(fields.x, fields.y);
    },
    [](whs31::common::Vector2 const& vector2) {
      return whs31::common::_glaze_impl::Vector2Fields {
        .x = vector2.x(),
        .y = vector2.y(),
      };
    }>;
};
