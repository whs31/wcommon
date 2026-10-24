#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/vector2.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Vector2 const vector)
{
  json = {{"x", vector.x()}, {"y", vector.y()}};
}

inline void from_json(nlohmann::json const& json, Vector2& vector)
{
  vector = Vector2::from_xy(json.at("x").get<double>(), json.at("y").get<double>());
}

WHS31_COMMON_NAMESPACE_END
