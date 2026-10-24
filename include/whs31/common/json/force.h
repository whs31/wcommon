#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/force.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Force const force)
{
  json = {{"newtons", force.as_newtons()}};
}

inline void from_json(nlohmann::json const& json, Force& force)
{
  force = Force::from_newtons(json.at("newtons").get<double>());
}

WHS31_COMMON_NAMESPACE_END
