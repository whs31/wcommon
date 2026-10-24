#pragma once

#include <optional>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/force.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct ForceFields
  {
    std::optional<double> newtons;
    std::optional<double> kilonewtons;
    std::optional<double> kilograms_force;
  };

  inline auto constexpr force_reading = QuantityReading<Force, ForceFields, 3, 3> {
    .fields =
      {
        field_unit(&ForceFields::newtons, &Force::from_newtons),
        field_unit(&ForceFields::kilonewtons, &Force::from_kilonewtons),
        field_unit(&ForceFields::kilograms_force, &Force::from_kilograms_force),
      },
    .literals =
      {
        literal_unit("n", &Force::from_newtons),
        literal_unit("kn", &Force::from_kilonewtons),
        literal_unit("kgf", &Force::from_kilograms_force),
      },
    .unknown_suffix = "a force literal needs one of the unit suffixes n, kn, kgf",
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Force: `{"newtons": magnitude}`.
 * @details Reading also accepts the magnitude in `kilonewtons` or `kilograms_force` instead. A
 * text format also reads a literal string, e.g. `"12.5n"`, with the suffix of a C++ literal:
 * `n`, `kn`, `kgf`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Force>
{
  static auto constexpr value = whs31::common::_glaze_impl::QuantityAdapter<
    whs31::common::_glaze_impl::force_reading>::value;
};
