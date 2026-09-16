#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/distance.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Distance const distance)
{
  json = {{"meters", distance.as_m()}};
}

inline void from_json(nlohmann::json const& json, Distance& distance)
{
  distance = Distance::from_meters(json.at("meters").get<float>());
}

RADAR_COMMON_NAMESPACE_END
