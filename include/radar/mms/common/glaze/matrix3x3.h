#pragma once

#include <array>
#include <cstddef>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/matrix3x3.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  using Matrix3x3Rows = std::array<std::array<double, 3>, 3>;
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Matrix3x3: an array of three rows, e.g. `[[1, 0, 0], [0, 1, 0], [0, 0,
 * 1]]`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Matrix3x3>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Matrix3x3& matrix,
      radar::mms::common::_glaze_impl::Matrix3x3Rows const& rows
    ) {
      auto const row = [&rows](std::size_t const index) {
        auto const [x, y, z] = rows.at(index);
        return radar::mms::common::Vector3(x, y, z);
      };
      matrix = radar::mms::common::Matrix3x3::from_rows(row(0), row(1), row(2));
    },
    [](radar::mms::common::Matrix3x3 const& matrix) {
      auto rows = radar::mms::common::_glaze_impl::Matrix3x3Rows {};
      for(auto index = std::size_t {0}; index < rows.size(); ++index) {
        auto const row = matrix.row(index);
        rows.at(index) = {row.x(), row.y(), row.z()};
      }
      return rows;
    }>;
};
