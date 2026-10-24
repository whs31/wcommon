#pragma once

#include <whs31/common/glaze/detail.h>
#include <whs31/common/point2.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct Point2Fields
  {
    double x = 0.0;
    double y = 0.0;
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Point2: `{"x": x, "y": y}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Point2>
{
  static auto constexpr value = glz::custom<
    [](whs31::common::Point2& point2, whs31::common::_glaze_impl::Point2Fields const& fields) {
      point2 = whs31::common::Point2::from_xy(fields.x, fields.y);
    },
    [](whs31::common::Point2 const& point2) {
      return whs31::common::_glaze_impl::Point2Fields {
        .x = point2.x(),
        .y = point2.y(),
      };
    }>;
};
