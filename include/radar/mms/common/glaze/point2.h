#pragma once

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/point2.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct Point2Fields
  {
    double x = 0.0;
    double y = 0.0;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Point2: `{"x": x, "y": y}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Point2>
{
  static auto constexpr value = glz::custom<
    [](
      radar::mms::common::Point2& point2,
      radar::mms::common::_glaze_impl::Point2Fields const& fields
    ) { point2 = radar::mms::common::Point2::from_xy(fields.x, fields.y); },
    [](radar::mms::common::Point2 const& point2) {
      return radar::mms::common::_glaze_impl::Point2Fields {
        .x = point2.x(),
        .y = point2.y(),
      };
    }>;
};
