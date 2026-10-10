#pragma once

#include <optional>

#include <radar/mms/common/glaze/detail.h>
#include <radar/mms/common/distance.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct DistanceFields
  {
    std::optional<double> meters;
    std::optional<double> nanometers;
    std::optional<double> micrometers;
    std::optional<double> millimeters;
    std::optional<double> centimeters;
    std::optional<double> decimeters;
    std::optional<double> hectometers;
    std::optional<double> kilometers;
    std::optional<double> inches;
    std::optional<double> feet;
    std::optional<double> yards;
    std::optional<double> furlongs;
    std::optional<double> miles;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Distance: `{"meters": magnitude}`.
 * @details Reading also accepts the magnitude in `nanometers`, `micrometers`, `millimeters`,
 * `centimeters`, `decimeters`, `hectometers`, `kilometers`, `inches`, `feet`, `yards`, `furlongs`
 * or `miles` instead.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Distance>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Distance& value,
      radar::mms::common::_glaze_impl::DistanceFields const& fields,
      glz::context& ctx
    ) {
      using Unit = radar::mms::common::_glaze_impl::UnitMagnitude<radar::mms::common::Distance>;
      radar::mms::common::_glaze_impl::read_in_unit(
        value,
        ctx,
        Unit {
          .magnitude = fields.meters,
          .from = &radar::mms::common::Distance::from_meters,
        },
        Unit {
          .magnitude = fields.nanometers,
          .from = &radar::mms::common::Distance::from_nanometers,
        },
        Unit {
          .magnitude = fields.micrometers,
          .from = &radar::mms::common::Distance::from_micrometers,
        },
        Unit {
          .magnitude = fields.millimeters,
          .from = &radar::mms::common::Distance::from_millimeters,
        },
        Unit {
          .magnitude = fields.centimeters,
          .from = &radar::mms::common::Distance::from_centimeters,
        },
        Unit {
          .magnitude = fields.decimeters,
          .from = &radar::mms::common::Distance::from_decimeters,
        },
        Unit {
          .magnitude = fields.hectometers,
          .from = &radar::mms::common::Distance::from_hectometers,
        },
        Unit {
          .magnitude = fields.kilometers,
          .from = &radar::mms::common::Distance::from_kilometers,
        },
        Unit {
          .magnitude = fields.inches,
          .from = &radar::mms::common::Distance::from_inches,
        },
        Unit {
          .magnitude = fields.feet,
          .from = &radar::mms::common::Distance::from_feet,
        },
        Unit {
          .magnitude = fields.yards,
          .from = &radar::mms::common::Distance::from_yards,
        },
        Unit {
          .magnitude = fields.furlongs,
          .from = &radar::mms::common::Distance::from_furlongs,
        },
        Unit {
          .magnitude = fields.miles,
          .from = &radar::mms::common::Distance::from_miles,
        }
      );
    },
    [](radar::mms::common::Distance const& value) {
      return radar::mms::common::_glaze_impl::DistanceFields {
        .meters = static_cast<double>(value),
      };
    }>;
};
