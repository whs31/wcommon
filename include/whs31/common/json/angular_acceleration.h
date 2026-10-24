#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/angular_acceleration.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, AngularAcceleration const angular_acceleration)
{
  json = {{"radians_per_second_squared", angular_acceleration.as_radians_per_second_squared()}};
}

inline void from_json(nlohmann::json const& json, AngularAcceleration& angular_acceleration)
{
  angular_acceleration = AngularAcceleration::from_radians_per_second_squared(
    json.at("radians_per_second_squared").get<double>()
  );
}

WHS31_COMMON_NAMESPACE_END
