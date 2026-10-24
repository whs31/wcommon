#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/conductance.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, Conductance const conductance)
{
  json = {{"siemens", conductance.as_siemens()}};
}

inline void from_json(nlohmann::json const& json, Conductance& conductance)
{
  conductance = Conductance::from_siemens(json.at("siemens").get<double>());
}

WHS31_COMMON_NAMESPACE_END
