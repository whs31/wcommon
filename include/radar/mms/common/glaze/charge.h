#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/charge.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct ChargeFields
  {
    std::optional<double> coulombs;
    std::optional<double> ampere_hours;
    std::optional<double> milliampere_hours;
  };

  inline constexpr auto charge_reading = QuantityReading<Charge, ChargeFields, 3, 2> {
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

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Charge: `{"coulombs": magnitude}`.
 * @details Reading also accepts the magnitude in `ampere_hours` or `milliampere_hours` instead.
 * A text format also reads a literal string, e.g. `"12.5ah"`, with the suffix of a C++ literal:
 * `ah`, `mah`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Charge>
{
  static constexpr auto value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::charge_reading>::value;
};
