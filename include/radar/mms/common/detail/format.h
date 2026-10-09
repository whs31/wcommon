#pragma once

#include <cstddef>
#include <array>

#include <fmt/format.h>

#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _format_impl // NOLINT(*-identifier-naming)
{
  /**
   * @brief Formats `values` as `(a, b, ...)`, applying the parsed spec of `formatter`, e.g.
   * `{:.3f}`, to every component.
   */
  template <std::size_t N>
  fmt::format_context::iterator format_components(
    fmt::formatter<double> const& formatter,
    std::array<double, N> const& values,
    fmt::format_context& ctx
  )
  {
    auto out = ctx.out();
    *out++ = '(';
    auto first = true;
    for(auto const value : values) {
      if(not first) {
        *out++ = ',';
        *out++ = ' ';
      }
      first = false;
      ctx.advance_to(out);
      out = formatter.format(value, ctx);
    }
    *out++ = ')';
    return out;
  }
}  // namespace _format_impl

RADAR_COMMON_NAMESPACE_END
