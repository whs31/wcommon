#pragma once

#include <array>
#include <cstdint>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/random.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct RandomStreamFields
  {
    std::array<std::uint64_t, 4> state {};
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of RandomStream: `{"state": [s0, s1, s2, s3]}`, e.g. for snapshots.
 * @details Reading fails with `constraint_violated` on an all-zero state, which the generator
 * cannot leave.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::RandomStream>
{
  static auto constexpr value = glz::custom<
    [](
      radar::mms::common::RandomStream& stream,
      radar::mms::common::_glaze_impl::RandomStreamFields const& fields,
      glz::context& ctx
    ) {
      if(fields.state == std::array<std::uint64_t, 4> {}) {
        radar::mms::common::_glaze_impl::reject(ctx, "invalid random stream: all-zero state");
        return;
      }
      stream = radar::mms::common::RandomStream {.state = fields.state};
    },
    [](radar::mms::common::RandomStream const& stream) {
      return radar::mms::common::_glaze_impl::RandomStreamFields {.state = stream.state};
    }>;
};
