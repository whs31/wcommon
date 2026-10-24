#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/capacitance.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Capacitance const capacitance)
{
  json = {{"farads", capacitance.as_farads()}};
}

inline void from_json(nlohmann::json const& json, Capacitance& capacitance)
{
  capacitance = Capacitance::from_farads(json.at("farads").get<double>());
}

WHS31_COMMON_NAMESPACE_END
