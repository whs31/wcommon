#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/point3.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Point3 const point)
{
  json = {{"x", point.x()}, {"y", point.y()}, {"z", point.z()}};
}

inline void from_json(nlohmann::json const& json, Point3& point)
{
  point = Point3::
    from_xyz(json.at("x").get<float>(), json.at("y").get<float>(), json.at("z").get<float>());
}

RADAR_COMMON_NAMESPACE_END
