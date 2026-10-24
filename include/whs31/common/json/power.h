#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/power.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Power const power)
{
  json = {{"watts", power.as_watts()}};
}

inline void from_json(nlohmann::json const& json, Power& power)
{
  power = Power::from_watts(json.at("watts").get<double>());
}

WHS31_COMMON_NAMESPACE_END
