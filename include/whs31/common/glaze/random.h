#pragma once

#include <array>
#include <cstdint>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/random.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct RandomStreamFields
  {
    std::array<std::uint64_t, 4> state {};
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of RandomStream: `{"state": [s0, s1, s2, s3]}`, e.g. for snapshots.
 * @details Reading fails with `constraint_violated` on an all-zero state, which the generator
 * cannot leave.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::RandomStream>
{
  static auto constexpr value = glz::custom<
    [](
      whs31::common::RandomStream& stream,
      whs31::common::_glaze_impl::RandomStreamFields const& fields,
      glz::context& ctx
    ) {
      if(fields.state == std::array<std::uint64_t, 4> {}) {
        whs31::common::_glaze_impl::reject(ctx, "invalid random stream: all-zero state");
        return;
      }
      stream = whs31::common::RandomStream {.state = fields.state};
    },
    [](whs31::common::RandomStream const& stream) {
      return whs31::common::_glaze_impl::RandomStreamFields {.state = stream.state};
    }>;
};
