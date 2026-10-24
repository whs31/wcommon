#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <whs31/common/angle.h>
#include <whs31/common/error.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Angle const angle)
{
  json = {{"angle", angle.degrees()}, {"units", "degrees"}};
}

inline void from_json(nlohmann::json const& json, Angle& angle)
{
  auto const units = json.at("units").get<std::string>();
  if(units == "degrees") {
    angle = Angle::from_degrees(json.at("angle").get<double>());
    return;
  }
  if(units == "radians") {
    angle = Angle::from_radians(json.at("angle").get<double>());
    return;
  }
  throw Error(
    "invalid angle conversion from json: units is {}, expected either degrees or radians",
    units
  );
}

WHS31_COMMON_NAMESPACE_END
