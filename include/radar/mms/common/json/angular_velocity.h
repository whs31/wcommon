#pragma once

#include <nlohmann/json.hpp>

#include <radar/mms/common/angular_velocity.h>

RADAR_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, AngularVelocity const angular_velocity)
{
  json = {{"radians_per_second", angular_velocity.as_radians_per_second()}};
}

inline void from_json(nlohmann::json const& json, AngularVelocity& angular_velocity)
{
  angular_velocity = AngularVelocity::from_radians_per_second(
    json.at("radians_per_second").get<double>()
  );
}

RADAR_COMMON_NAMESPACE_END
