#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/quaternion.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Quaternion const& quaternion)
{
  json = {
    {"w", quaternion.w()},
    {"x", quaternion.x()},
    {"y", quaternion.y()},
    {"z", quaternion.z()},
  };
}

inline void from_json(nlohmann::json const& json, Quaternion& quaternion)
{
  quaternion = Quaternion::from_wxyz(
    json.at("w").get<double>(),
    json.at("x").get<double>(),
    json.at("y").get<double>(),
    json.at("z").get<double>()
  );
}

RADAR_COMMON_NAMESPACE_END
