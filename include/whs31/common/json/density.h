#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/density.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Density const density)
{
  json = {{"kilograms_per_cubic_meter", density.as_kilograms_per_cubic_meter()}};
}

inline void from_json(nlohmann::json const& json, Density& density)
{
  density = Density::from_kilograms_per_cubic_meter(
    json.at("kilograms_per_cubic_meter").get<double>()
  );
}

WHS31_COMMON_NAMESPACE_END
