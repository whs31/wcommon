#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/resistance.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Resistance const resistance)
{
  json = {{"ohms", resistance.as_ohms()}};
}

inline void from_json(nlohmann::json const& json, Resistance& resistance)
{
  resistance = Resistance::from_ohms(json.at("ohms").get<double>());
}

RADAR_COMMON_NAMESPACE_END
