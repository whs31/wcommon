#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/voltage.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Voltage const voltage)
{
  json = {{"volts", voltage.as_volts()}};
}

inline void from_json(nlohmann::json const& json, Voltage& voltage)
{
  voltage = Voltage::from_volts(json.at("volts").get<double>());
}

WHS31_COMMON_NAMESPACE_END
