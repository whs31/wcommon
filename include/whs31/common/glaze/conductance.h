#pragma once

#include <optional>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/conductance.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct ConductanceFields
  {
    std::optional<double> siemens;
  };

  inline auto constexpr conductance_reading = QuantityReading<
    Conductance,
    ConductanceFields,
    1,
    1> {
    .fields =
      {
        field_unit(&ConductanceFields::siemens, &Conductance::from_siemens),
      },
    .literals =
      {
        literal_unit("siemens", &Conductance::from_siemens),
      },
    .unknown_suffix = "a conductance literal needs one of the unit suffixes siemens",
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Conductance: `{"siemens": magnitude}`.
 * @details A text format also reads a literal string, e.g. `"12.5siemens"`, with the suffix of
 * a C++ literal: `siemens`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Conductance>
{
  static auto constexpr value = whs31::common::_glaze_impl::QuantityAdapter<
    whs31::common::_glaze_impl::conductance_reading>::value;
};
