#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/current.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct CurrentFields
  {
    std::optional<double> amperes;
    std::optional<double> milliamperes;
  };

  inline auto constexpr current_reading = QuantityReading<Current, CurrentFields, 2, 2> {
    .fields =
      {
        field_unit(&CurrentFields::amperes, &Current::from_amperes),
        field_unit(&CurrentFields::milliamperes, &Current::from_milliamperes),
      },
    .literals =
      {
        literal_unit("a", &Current::from_amperes),
        literal_unit("ma", &Current::from_milliamperes),
      },
    .unknown_suffix = "a current literal needs one of the unit suffixes a, ma",
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Current: `{"amperes": magnitude}`.
 * @details Reading also accepts the magnitude in `milliamperes` instead. A text format also
 * reads a literal string, e.g. `"12.5a"`, with the suffix of a C++ literal: `a`, `ma`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Current>
{
  static auto constexpr value = radar::mms::common::_glaze_impl::QuantityAdapter<
    radar::mms::common::_glaze_impl::current_reading>::value;
};
