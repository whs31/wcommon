#pragma once

#include <cstddef>
#include <algorithm>

#include <nlohmann/json.hpp>

#include <whs31/common/error.h>
#include <whs31/common/matrix3x3.h>

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Serializes the matrix as an array of three rows, e.g. `[[1, 0, 0], [0, 1, 0], [0, 0, 1]]`.
 * @ingroup core_math
 */
inline void to_json(nlohmann::json& json, Matrix3x3 const& matrix)
{
  json = nlohmann::json::array();
  for(auto index = std::size_t {0}; index < 3; ++index) {
    auto const row = matrix.row(index);
    json.push_back({row.x(), row.y(), row.z()});
  }
}

inline void from_json(nlohmann::json const& json, Matrix3x3& matrix)
{
  auto const is_row = [](nlohmann::json const& row) { return row.is_array() and row.size() == 3; };
  if(not json.is_array() or json.size() != 3 or not std::ranges::all_of(json, is_row))
    throw InvalidArgumentError("invalid matrix conversion from json: expected 3 rows of 3 numbers");

  auto const row = [&json](std::size_t const index) {
    auto const& values = json.at(index);
    return Vector3(
      values.at(0).get<double>(),
      values.at(1).get<double>(),
      values.at(2).get<double>()
    );
  };
  matrix = Matrix3x3::from_rows(row(0), row(1), row(2));
}

WHS31_COMMON_NAMESPACE_END
