#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/glaze/ip.h>
#include <whs31/common/sockaddr.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct SockAddrV4Fields
  {
    Ipv4Addr ip;
    std::uint16_t port = 0;
  };

  struct SockAddrV6Fields
  {
    Ipv6Addr ip;
    std::uint16_t port = 0;
    std::uint32_t flow_info = 0;
    std::uint32_t scope_id = 0;
  };

  // The address of a SockAddr, whose IP version is given by the sibling "family" key.
  struct SockAddrAddressFields
  {
    std::string ip;
    std::uint16_t port = 0;
    std::optional<std::uint32_t> flow_info;
    std::optional<std::uint32_t> scope_id;
  };

  struct SockAddrFields
  {
    std::string family;
    SockAddrAddressFields address;
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of SockAddrV4: `{"ip": "192.168.0.1", "port": 8080}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::SockAddrV4>
{
  static auto constexpr value = glz::custom<
    [](
      whs31::common::SockAddrV4& address,
      whs31::common::_glaze_impl::SockAddrV4Fields const& fields
    ) { address = whs31::common::SockAddrV4(fields.ip, fields.port); },
    [](whs31::common::SockAddrV4 const& address) {
      return whs31::common::_glaze_impl::SockAddrV4Fields {
        .ip = address.ip(),
        .port = static_cast<std::uint16_t>(address.port()),
      };
    }>;
};

/**
 * @brief Glaze metadata of SockAddrV6: `{"ip": "::1", "port": 8080, "flow_info": 0, "scope_id":
 * 0}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::SockAddrV6>
{
  static auto constexpr value = glz::custom<
    [](
      whs31::common::SockAddrV6& address,
      whs31::common::_glaze_impl::SockAddrV6Fields const& fields
    ) {
      address = whs31::common::
        SockAddrV6(fields.ip, fields.port, fields.flow_info, fields.scope_id);
    },
    [](whs31::common::SockAddrV6 const& address) {
      return whs31::common::_glaze_impl::SockAddrV6Fields {
        .ip = address.ip(),
        .port = static_cast<std::uint16_t>(address.port()),
        .flow_info = address.flow_info(),
        .scope_id = address.scope_id(),
      };
    }>;
};

/**
 * @brief Glaze metadata of SockAddr: `{"family": "ipv4" | "ipv6", "address": {...}}`.
 * @details The address has the schema of SockAddrV4 or SockAddrV6. Reading fails with
 * `constraint_violated` on an unknown family, an invalid IP, or IPv6 fields in an IPv4 address.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::SockAddr>
{
  static auto constexpr value = glz::custom<
    [](
      whs31::common::SockAddr& address,
      whs31::common::_glaze_impl::SockAddrFields const& fields,
      glz::context& ctx
    ) {
      namespace rc = whs31::common;
      auto const& inner = fields.address;
      try {
        if(fields.family == "ipv4") {
          if(inner.flow_info or inner.scope_id) {
            rc::_glaze_impl::reject(ctx, "ipv4 socket address has no flow_info or scope_id");
            return;
          }
          address = rc::SockAddr(rc::SockAddrV4(rc::Ipv4Addr::parse(inner.ip), inner.port));
        }
        else if(fields.family == "ipv6") {
          address = rc::SockAddr(
            rc::SockAddrV6(
              rc::Ipv6Addr::parse(inner.ip),
              inner.port,
              inner.flow_info.value_or(0),
              inner.scope_id.value_or(0)
            )
          );
        }
        else {
          rc::_glaze_impl::reject(ctx, "invalid socket address family: expected ipv4 or ipv6");
        }
      } catch(rc::IpError const&) {
        rc::_glaze_impl::reject(ctx, "invalid socket address ip");
      }
    },
    [](whs31::common::SockAddr const& address) {
      namespace rc = whs31::common;
      if(address.is_ipv4()) {
        auto const& v4 = address.as_v4();
        return rc::_glaze_impl::SockAddrFields {
          .family = "ipv4",
          .address = {.ip = v4.ip().to_string(), .port = static_cast<std::uint16_t>(v4.port())},
        };
      }
      auto const& v6 = address.as_v6();
      return rc::_glaze_impl::SockAddrFields {
        .family = "ipv6",
        .address = {
          .ip = v6.ip().to_string(),
          .port = static_cast<std::uint16_t>(v6.port()),
          .flow_info = v6.flow_info(),
          .scope_id = v6.scope_id(),
        },
      };
    }>;
};
