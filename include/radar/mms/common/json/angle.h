#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <radar/mms/common/angle.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Angle const angle)
{
  json = {{"angle", angle.degrees()}, {"units", "degrees"}};
}

inline void from_json(nlohmann::json const& json, Angle& angle)
{
  auto const units = json.at("units").get<std::string>();
  if(units == "degrees") {
    angle = Angle::from_degrees(json.at("angle").get<float>());
    return;
  }
  if(units == "radians") {
    angle = Angle::from_radians(json.at("angle").get<float>());
    return;
  }
  throw Error(
    "invalid angle conversion from json: units is {}, expected either degrees or radians",
    units
  );
}

RADAR_COMMON_NAMESPACE_END
