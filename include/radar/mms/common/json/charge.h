#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/charge.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Charge const charge)
{
  json = {{"coulombs", charge.as_coulombs()}};
}

inline void from_json(nlohmann::json const& json, Charge& charge)
{
  charge = Charge::from_coulombs(json.at("coulombs").get<double>());
}

RADAR_COMMON_NAMESPACE_END
