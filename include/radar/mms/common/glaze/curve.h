#pragma once

#include <cstddef>
#include <vector>

#include <radar/mms/common/curve.h>
#include <radar/mms/common/error.h>
#include <radar/mms/common/glaze/detail.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  template <CurveAxis X, CurveAxis Y>
  struct CurveFields
  {
    std::vector<X> arguments;
    std::vector<Y> values;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Curve: `{"arguments": [...], "values": [...]}`.
 * @details Reading validates the points and fails with `constraint_violated` if they do not form a
 * valid curve. Quantity axes need their own glaze adapters, e.g. `radar/mms/common/glaze/angle.h`.
 * @ingroup core_glaze
 */
template <radar::mms::common::CurveAxis X, radar::mms::common::CurveAxis Y, std::size_t MaxPoints>
  requires(MaxPoints > 0)
struct glz::meta<radar::mms::common::Curve<X, Y, MaxPoints>>
{
  using Curve = radar::mms::common::Curve<X, Y, MaxPoints>;
  using Fields = radar::mms::common::_glaze_impl::CurveFields<X, Y>;

  static auto constexpr value = glz::custom<
    [](Curve& curve, Fields const& fields, glz::context& ctx) {
      try {
        curve = Curve(fields.arguments, fields.values);
      } catch(radar::mms::common::Error const&) {
        radar::mms::common::_glaze_impl::reject(ctx, "invalid curve points");
      }
    },
    [](Curve const& curve) {
      return Fields {
        .arguments = std::vector<X>(curve.arguments().begin(), curve.arguments().end()),
        .values = std::vector<Y>(curve.values().begin(), curve.values().end()),
      };
    }>;
};
