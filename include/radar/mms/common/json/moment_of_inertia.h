#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/moment_of_inertia.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, MomentOfInertia const moment_of_inertia)
{
  json = {{"kilogram_square_meters", moment_of_inertia.as_kilogram_square_meters()}};
}

inline void from_json(nlohmann::json const& json, MomentOfInertia& moment_of_inertia)
{
  moment_of_inertia = MomentOfInertia::from_kilogram_square_meters(
    json.at("kilogram_square_meters").get<double>()
  );
}

RADAR_COMMON_NAMESPACE_END
