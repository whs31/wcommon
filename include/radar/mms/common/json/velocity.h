#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/velocity.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Velocity const velocity)
{
  json = {{"mps", velocity.as_mps()}};
}

inline void from_json(nlohmann::json const& json, Velocity& velocity)
{
  velocity = Velocity::from_mps(json.at("mps").get<double>());
}

RADAR_COMMON_NAMESPACE_END
