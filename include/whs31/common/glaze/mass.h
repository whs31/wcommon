#pragma once

#include <optional>

#include <whs31/common/glaze/detail.h>
#include <whs31/common/mass.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct MassFields
  {
    std::optional<double> kilograms;
    std::optional<double> grams;
  };

  inline auto constexpr mass_reading = QuantityReading<Mass, MassFields, 2, 2> {
    .fields =
      {
        field_unit(&MassFields::kilograms, &Mass::from_kilograms),
        field_unit(&MassFields::grams, &Mass::from_grams),
      },
    .literals =
      {
        literal_unit("kg", &Mass::from_kilograms),
        literal_unit("g", &Mass::from_grams),
      },
    .unknown_suffix = "a mass literal needs one of the unit suffixes kg, g",
  };
}  // namespace _glaze_impl

WHS31_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Mass: `{"kilograms": magnitude}`.
 * @details Reading also accepts the magnitude in `grams` instead. A text format also reads a
 * literal string, e.g. `"12.5kg"`, with the suffix of a C++ literal: `kg`, `g`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<whs31::common::Mass>
{
  static auto constexpr value = whs31::common::_glaze_impl::QuantityAdapter<
    whs31::common::_glaze_impl::mass_reading>::value;
};
