#pragma once

#include <optional>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/torque.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct TorqueFields
  {
    std::optional<double> newton_meters;
  };

  inline auto constexpr torque_reading = QuantityReading<Torque, TorqueFields, 1, 1> {
    .fields =
      {
        field_unit(&TorqueFields::newton_meters, &Torque::from_newton_meters),
      },
    .literals =
      {
        literal_unit("nm", &Torque::from_newton_meters),
      },
    .unknown_suffix = "a torque literal needs one of the unit suffixes nm",
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Torque: `{"newton_meters": magnitude}`.
 * @details A text format also reads a literal string, e.g. `"12.5nm"`, with the suffix of a C++
 * literal: `nm`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Torque>
{
  static auto constexpr value = whs31::common::_glaze_impl::QuantityAdapter<
    whs31::common::_glaze_impl::torque_reading>::value;
};
