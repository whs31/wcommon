#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/charge.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Charge const charge)
{
  json = {{"coulombs", charge.as_coulombs()}};
}

inline void from_json(nlohmann::json const& json, Charge& charge)
{
  charge = Charge::from_coulombs(json.at("coulombs").get<double>());
}

WHS31_COMMON_NAMESPACE_END
