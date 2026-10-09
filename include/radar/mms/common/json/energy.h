#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/energy.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Energy const energy)
{
  json = {{"joules", energy.as_joules()}};
}

inline void from_json(nlohmann::json const& json, Energy& energy)
{
  energy = Energy::from_joules(json.at("joules").get<double>());
}

RADAR_COMMON_NAMESPACE_END
