#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/point2.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Point2 const point)
{
  json = {{"x", point.x()}, {"y", point.y()}};
}

inline void from_json(nlohmann::json const& json, Point2& point)
{
  point = Point2::from_xy(json.at("x").get<double>(), json.at("y").get<double>());
}

RADAR_COMMON_NAMESPACE_END
