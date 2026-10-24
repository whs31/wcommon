#pragma once

#include <string>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/uuid.h>

/**
 * @brief Glaze metadata of Uuid: its canonical string, e.g.
 * `"67e55044-10b1-426f-9247-bb680e5fe0c8"`.
 * @details Reading fails with `constraint_violated` on an invalid string.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Uuid>
{
  static auto constexpr value = glz::custom<
    [](whs31::common::Uuid& uuid, std::string const& text, glz::context& ctx) {
      try {
        uuid = whs31::common::Uuid(text);
      } catch(whs31::common::UuidError const&) {
        whs31::common::_glaze_impl::reject(ctx, "invalid uuid string");
      }
    },
    [](whs31::common::Uuid const& uuid) { return uuid.to_string(); }>;
};
