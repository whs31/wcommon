#pragma once

#include <nlohmann/json.hpp>

#include <whs31/common/magnetic_flux_density.h>

WHS31_COMMON_NAMESPACE_BEGIN

inline void to_json(nlohmann::json& json, MagneticFluxDensity const magnetic_flux_density)
{
  json = {{"teslas", magnetic_flux_density.as_teslas()}};
}

inline void from_json(nlohmann::json const& json, MagneticFluxDensity& magnetic_flux_density)
{
  magnetic_flux_density = MagneticFluxDensity::from_teslas(json.at("teslas").get<double>());
}

WHS31_COMMON_NAMESPACE_END
