#pragma once

#include <string>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/ip.h>

/**
 * @brief Glaze metadata of Ipv4Addr: its dotted string, e.g. `"192.168.0.1"`.
 * @details Reading fails with `constraint_violated` on an invalid address.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Ipv4Addr>
{
  static auto constexpr value = glz::custom<
    [](whs31::common::Ipv4Addr& address, std::string const& text, glz::context& ctx) {
      try {
        address = whs31::common::Ipv4Addr::parse(text);
      } catch(whs31::common::IpError const&) {
        whs31::common::_glaze_impl::reject(ctx, "invalid ipv4 address");
      }
    },
    [](whs31::common::Ipv4Addr const& address) { return address.to_string(); }>;
};

/**
 * @brief Glaze metadata of Ipv6Addr: its string, e.g. `"::1"`.
 * @details Reading fails with `constraint_violated` on an invalid address.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Ipv6Addr>
{
  static auto constexpr value = glz::custom<
    [](whs31::common::Ipv6Addr& address, std::string const& text, glz::context& ctx) {
      try {
        address = whs31::common::Ipv6Addr::parse(text);
      } catch(whs31::common::IpError const&) {
        whs31::common::_glaze_impl::reject(ctx, "invalid ipv6 address");
      }
    },
    [](whs31::common::Ipv6Addr const& address) { return address.to_string(); }>;
};
