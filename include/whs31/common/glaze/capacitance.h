#pragma once

#include <optional>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/capacitance.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct CapacitanceFields
  {
    std::optional<double> farads;
    std::optional<double> microfarads;
  };

  inline auto constexpr capacitance_reading = QuantityReading<
    Capacitance,
    CapacitanceFields,
    2,
    2> {
    .fields =
      {
        field_unit(&CapacitanceFields::farads, &Capacitance::from_farads),
        field_unit(&CapacitanceFields::microfarads, &Capacitance::from_microfarads),
      },
    .literals =
      {
        literal_unit("f", &Capacitance::from_farads),
        literal_unit("uf", &Capacitance::from_microfarads),
      },
    .unknown_suffix = "a capacitance literal needs one of the unit suffixes f, uf",
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Capacitance: `{"farads": magnitude}`.
 * @details Reading also accepts the magnitude in `microfarads` instead. A text format also
 * reads a literal string, e.g. `"12.5f"`, with the suffix of a C++ literal: `f`, `uf`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Capacitance>
{
  static auto constexpr value = whs31::common::_glaze_impl::QuantityAdapter<
    whs31::common::_glaze_impl::capacitance_reading>::value;
};
