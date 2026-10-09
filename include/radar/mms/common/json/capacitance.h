#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/capacitance.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Capacitance const capacitance)
{
  json = {{"farads", capacitance.as_farads()}};
}

inline void from_json(nlohmann::json const& json, Capacitance& capacitance)
{
  capacitance = Capacitance::from_farads(json.at("farads").get<double>());
}

RADAR_COMMON_NAMESPACE_END
