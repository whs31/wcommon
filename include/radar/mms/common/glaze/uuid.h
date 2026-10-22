#pragma once

#include <string>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/uuid.h>

/**
 * @brief Glaze metadata of Uuid: its canonical string, e.g.
 * `"67e55044-10b1-426f-9247-bb680e5fe0c8"`.
 * @details Reading fails with `constraint_violated` on an invalid string.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Uuid>
{
  static auto constexpr value = glz::custom<
    [](radar::mms::common::Uuid& uuid, std::string const& text, glz::context& ctx) {
      try {
        uuid = radar::mms::common::Uuid(text);
      } catch(radar::mms::common::UuidError const&) {
        radar::mms::common::_glaze_impl::reject(ctx, "invalid uuid string");
      }
    },
    [](radar::mms::common::Uuid const& uuid) { return uuid.to_string(); }>;
};
