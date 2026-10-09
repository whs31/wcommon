#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/torque.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Torque const torque)
{
  json = {{"newton_meters", torque.as_newton_meters()}};
}

inline void from_json(nlohmann::json const& json, Torque& torque)
{
  torque = Torque::from_newton_meters(json.at("newton_meters").get<double>());
}

RADAR_COMMON_NAMESPACE_END
