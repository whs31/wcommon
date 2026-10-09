#pragma once

#include <cstddef>
#include <vector>

#include <nlohmann/json.hpp>

#include <radar/mms/common/curve.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Serializes the curve as `{"arguments": [...], "values": [...]}`.
 * @details Quantity axes need their own JSON adapters, e.g. `radar/mms/common/json/angle.h`.
 * @ingroup core_math
 */
template <CurveAxis X, CurveAxis Y, std::size_t MaxPoints>
void to_json(nlohmann::json& json, Curve<X, Y, MaxPoints> const& curve)
{
  json = {
    {"arguments", std::vector<X>(curve.arguments().begin(), curve.arguments().end())},
    {"values", std::vector<Y>(curve.values().begin(), curve.values().end())},
  };
}

/**
 * @brief Deserializes and validates a curve.
 * @throws InvalidArgumentError if the points do not form a valid curve.
 */
template <CurveAxis X, CurveAxis Y, std::size_t MaxPoints>
void from_json(nlohmann::json const& json, Curve<X, Y, MaxPoints>& curve)
{
  auto const arguments = json.at("arguments").get<std::vector<X>>();
  auto const values = json.at("values").get<std::vector<Y>>();
  curve = Curve<X, Y, MaxPoints>(arguments, values);
}

RADAR_COMMON_NAMESPACE_END
