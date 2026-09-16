#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/vector3.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Vector3 const vector)
{
  json = {{"x", vector.x()}, {"y", vector.y()}, {"z", vector.z()}};
}

inline void from_json(nlohmann::json const& json, Vector3& vector)
{
  vector = Vector3::
    from_xyz(json.at("x").get<float>(), json.at("y").get<float>(), json.at("z").get<float>());
}

RADAR_COMMON_NAMESPACE_END
