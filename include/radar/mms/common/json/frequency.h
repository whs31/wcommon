#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/frequency.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Frequency const frequency)
{
  json = {{"hertz", frequency.as_hertz()}};
}

inline void from_json(nlohmann::json const& json, Frequency& frequency)
{
  frequency = Frequency::from_hertz(json.at("hertz").get<double>());
}

RADAR_COMMON_NAMESPACE_END
