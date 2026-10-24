#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/point2.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Point2 const point)
{
  json = {{"x", point.x()}, {"y", point.y()}};
}

inline void from_json(nlohmann::json const& json, Point2& point)
{
  point = Point2::from_xy(json.at("x").get<double>(), json.at("y").get<double>());
}

WHS31_COMMON_NAMESPACE_END
