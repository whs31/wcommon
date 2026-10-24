#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/temperature.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Temperature const temperature)
{
  json = {{"kelvin", temperature.as_kelvin()}};
}

inline void from_json(nlohmann::json const& json, Temperature& temperature)
{
  temperature = Temperature::from_kelvin(json.at("kelvin").get<double>());
}

WHS31_COMMON_NAMESPACE_END
