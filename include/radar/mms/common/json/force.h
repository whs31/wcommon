#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/force.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Force const force)
{
  json = {{"newtons", force.as_newtons()}};
}

inline void from_json(nlohmann::json const& json, Force& force)
{
  force = Force::from_newtons(json.at("newtons").get<double>());
}

RADAR_COMMON_NAMESPACE_END
