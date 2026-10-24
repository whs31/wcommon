#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/mass.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Mass const mass)
{
  json = {{"kilograms", mass.as_kilograms()}};
}

inline void from_json(nlohmann::json const& json, Mass& mass)
{
  mass = Mass::from_kilograms(json.at("kilograms").get<double>());
}

WHS31_COMMON_NAMESPACE_END
