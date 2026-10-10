#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/conductance.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct ConductanceFields
  {
    std::optional<double> siemens;
  };

  inline constexpr auto
    conductance_reading = QuantityReading<Conductance, ConductanceFields, 1, 1> {
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

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Conductance: `{"siemens": magnitude}`.
 * @details A text format also reads a literal string, e.g. `"12.5siemens"`, with the suffix of
 * a C++ literal: `siemens`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Conductance>
{
  static constexpr auto value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::conductance_reading>::value;
};
