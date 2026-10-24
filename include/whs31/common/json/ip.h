#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <whs31/common/ip.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Ipv4Addr const& address) { json = address.to_string(); }

inline void from_json(nlohmann::json const& json, Ipv4Addr& address)
{
  address = Ipv4Addr::parse(json.get<std::string>());
}

inline void to_json(nlohmann::json& json, Ipv6Addr const& address) { json = address.to_string(); }

inline void from_json(nlohmann::json const& json, Ipv6Addr& address)
{
  address = Ipv6Addr::parse(json.get<std::string>());
}

WHS31_COMMON_NAMESPACE_END
