#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/euler_angles.h>
#include <radar/mms/common/json/angle.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, EulerAngles const& angles)
{
  json = {{"roll", angles.roll()}, {"pitch", angles.pitch()}, {"yaw", angles.yaw()}};
}

inline void from_json(nlohmann::json const& json, EulerAngles& angles)
{
  angles = EulerAngles(
    json.at("roll").get<Angle>(),
    json.at("pitch").get<Angle>(),
    json.at("yaw").get<Angle>()
  );
}

RADAR_COMMON_NAMESPACE_END
