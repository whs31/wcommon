#pragma once

#include <cstddef>
#include <vector>

#include <whs31/common/curve.h>
#include <whs31/common/error.h>
#include <whs31/common/glaze/detail.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  template <CurveAxis X, CurveAxis Y>
  struct CurveFields
  {
    std::vector<X> arguments;
    std::vector<Y> values;
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Curve: `{"arguments": [...], "values": [...]}`.
 * @details Reading validates the points and fails with `constraint_violated` if they do not form a
 * valid curve. Quantity axes need their own glaze adapters, e.g. `whs31/common/glaze/angle.h`.
 * @ingroup core_glaze
 */
template <whs31::common::CurveAxis X, whs31::common::CurveAxis Y, std::size_t MaxPoints>
  requires(MaxPoints > 0)
struct glz::meta<whs31::common::Curve<X, Y, MaxPoints>>
{
  using Curve = whs31::common::Curve<X, Y, MaxPoints>;
  using Fields = whs31::common::_glaze_impl::CurveFields<X, Y>;

  static auto constexpr value = glz::custom<
    [](Curve& curve, Fields const& fields, glz::context& ctx) {
      try {
        curve = Curve(fields.arguments, fields.values);
      } catch(whs31::common::Error const&) {
        whs31::common::_glaze_impl::reject(ctx, "invalid curve points");
      }
    },
    [](Curve const& curve) {
      return Fields {
        .arguments = std::vector<X>(curve.arguments().begin(), curve.arguments().end()),
        .values = std::vector<Y>(curve.values().begin(), curve.values().end()),
      };
    }>;
};
