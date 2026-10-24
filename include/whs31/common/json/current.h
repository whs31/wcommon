#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/current.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Current const current)
{
  json = {{"amperes", current.as_amperes()}};
}

inline void from_json(nlohmann::json const& json, Current& current)
{
  current = Current::from_amperes(json.at("amperes").get<double>());
}

WHS31_COMMON_NAMESPACE_END
