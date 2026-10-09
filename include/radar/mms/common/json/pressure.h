#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/pressure.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Pressure const pressure)
{
  json = {{"pascals", pressure.as_pascals()}};
}

inline void from_json(nlohmann::json const& json, Pressure& pressure)
{
  pressure = Pressure::from_pascals(json.at("pascals").get<double>());
}

RADAR_COMMON_NAMESPACE_END
