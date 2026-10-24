#pragma once

#include <optional>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/charge.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct ChargeFields
  {
    std::optional<double> coulombs;
    std::optional<double> ampere_hours;
    std::optional<double> milliampere_hours;
  };

  inline auto constexpr charge_reading = QuantityReading<Charge, ChargeFields, 3, 2> {
    .fields =
      {
        field_unit(&ChargeFields::coulombs, &Charge::from_coulombs),
        field_unit(&ChargeFields::ampere_hours, &Charge::from_ampere_hours),
        field_unit(&ChargeFields::milliampere_hours, &Charge::from_milliampere_hours),
      },
    .literals =
      {
        literal_unit("ah", &Charge::from_ampere_hours),
        literal_unit("mah", &Charge::from_milliampere_hours),
      },
    .unknown_suffix = "a charge literal needs one of the unit suffixes ah, mah",
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Charge: `{"coulombs": magnitude}`.
 * @details Reading also accepts the magnitude in `ampere_hours` or `milliampere_hours` instead.
 * A text format also reads a literal string, e.g. `"12.5ah"`, with the suffix of a C++ literal:
 * `ah`, `mah`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Charge>
{
  static auto constexpr value = whs31::common::_glaze_impl::QuantityAdapter<
    whs31::common::_glaze_impl::charge_reading>::value;
};
