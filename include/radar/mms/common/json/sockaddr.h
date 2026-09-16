#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include <radar/mms/common/json/ip.h>
#include <radar/mms/common/sockaddr.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _json_sockaddr_impl  // NOLINT(readability-identifier-naming)
{
  template <typename T>
  [[nodiscard]] T get_unsigned(nlohmann::json const& object, std::string_view const name)
  {
    auto const& value = object.at(name);
    auto result = std::uint64_t {};

    if(value.is_number_unsigned()) {
      result = value.get<std::uint64_t>();
    }
    else if(value.is_number_integer()) {
      auto const signed_result = value.get<std::int64_t>();
      if(signed_result < 0)
        throw IpError("json field '{}' must be an unsigned integer", name);
      result = static_cast<std::uint64_t>(signed_result);
    }
    else {
      throw IpError("json field '{}' must be an unsigned integer", name);
    }

    if(result > std::numeric_limits<T>::max())
      throw IpError("json field '{}' is out of range", name);
    return static_cast<T>(result);
  }
}  // namespace _json_sockaddr_impl

inline void to_json(nlohmann::json& json, SockAddrV4 const& address)
{
  json = nlohmann::json {
    {"ip", address.ip()},
    {"port", address.port()},
  };
}

inline void from_json(nlohmann::json const& json, SockAddrV4& address)
{
  address = SockAddrV4(
    json.at("ip").get<Ipv4Addr>(),
    _json_sockaddr_impl::get_unsigned<std::uint16_t>(json, "port")
  );
}

inline void to_json(nlohmann::json& json, SockAddrV6 const& address)
{
  json = nlohmann::json {
    {"ip", address.ip()},
    {"port", address.port()},
    {"flow_info", address.flow_info()},
    {"scope_id", address.scope_id()},
  };
}

inline void from_json(nlohmann::json const& json, SockAddrV6& address)
{
  address = SockAddrV6(
    json.at("ip").get<Ipv6Addr>(),
    _json_sockaddr_impl::get_unsigned<std::uint16_t>(json, "port"),
    _json_sockaddr_impl::get_unsigned<std::uint32_t>(json, "flow_info"),
    _json_sockaddr_impl::get_unsigned<std::uint32_t>(json, "scope_id")
  );
}

inline void to_json(nlohmann::json& json, SockAddr const& address)
{
  if(address.is_ipv4()) {
    json = nlohmann::json {
      {"family", "ipv4"},
      {"address", address.as_v4()},
    };
  }
  else {
    json = nlohmann::json {
      {"family", "ipv6"},
      {"address", address.as_v6()},
    };
  }
}

inline void from_json(nlohmann::json const& json, SockAddr& address)
{
  auto const family = json.at("family").get<std::string>();
  if(family == "ipv4") {
    address = SockAddr(json.at("address").get<SockAddrV4>());
    return;
  }
  if(family == "ipv6") {
    address = SockAddr(json.at("address").get<SockAddrV6>());
    return;
  }
  throw IpError("invalid socket address family: {}", family);
}

RADAR_COMMON_NAMESPACE_END
