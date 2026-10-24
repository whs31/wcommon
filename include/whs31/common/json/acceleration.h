#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/acceleration.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Acceleration const acceleration)
{
  json = {{"meters_per_second_squared", acceleration.as_mps2()}};
}

inline void from_json(nlohmann::json const& json, Acceleration& acceleration)
{
  acceleration = Acceleration::from_mps2(json.at("meters_per_second_squared").get<double>());
}

WHS31_COMMON_NAMESPACE_END
