#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/geodesy.h>
#include <whs31/common/json/angle.h>
#include <whs31/common/json/distance.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, GeodeticPosition const& position)
{
  json = {
    {"latitude", position.latitude},
    {"longitude", position.longitude},
    {"altitude_msl", position.altitude_msl},
  };
}

inline void from_json(nlohmann::json const& json, GeodeticPosition& position)
{
  position = {
    .latitude = json.at("latitude").get<Angle>(),
    .longitude = json.at("longitude").get<Angle>(),
    .altitude_msl = json.at("altitude_msl").get<Distance>(),
  };
}

WHS31_COMMON_NAMESPACE_END
