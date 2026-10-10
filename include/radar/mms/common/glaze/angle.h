#pragma once

#include <optional>
#include <string>

#include <radar/mms/common/angle.h>
#include <radar/mms/common/glaze/detail.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct AngleFields
  {
    std::optional<double> angle;
    std::optional<std::string> units;
    std::optional<double> degrees;
    std::optional<double> radians;
  };

  inline void read_angle(Angle& angle, AngleFields const& fields, glz::context& ctx) noexcept
  {
    if(not fields.angle.has_value()) {
      if(fields.units.has_value())
        reject(ctx, "angle units without an angle");
      else
        read_in_unit(
          angle,
          ctx,
          UnitMagnitude<Angle> {
            .magnitude = fields.degrees,
            .from = &Angle::from_degrees,
          },
          UnitMagnitude<Angle> {
            .magnitude = fields.radians,
            .from = &Angle::from_radians,
          }
        );
      return;
    }
    if(fields.degrees.has_value() or fields.radians.has_value())
      reject(ctx, "an angle must hold its magnitude in exactly one unit");
    else if(fields.units == "degrees")
      angle = Angle::from_degrees(*fields.angle);
    else if(fields.units == "radians")
      angle = Angle::from_radians(*fields.angle);
    else
      reject(ctx, "invalid angle units: expected either degrees or radians");
  }
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Angle: `{"angle": degrees, "units": "degrees"}`.
 * @details Reading also accepts `"units": "radians"`, and the shorter `{"degrees": magnitude}` or
 * `{"radians": magnitude}`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Angle>
{
  static constexpr auto value = glz::custom<
    [](
      radar::mms::common::Angle& angle,
      radar::mms::common::_glaze_impl::AngleFields const& fields,
      glz::context& ctx
    ) { radar::mms::common::_glaze_impl::read_angle(angle, fields, ctx); },
    [](radar::mms::common::Angle const& angle) {
      return radar::mms::common::_glaze_impl::AngleFields {
        .angle = angle.degrees(),
        .units = "degrees",
      };
    }>;
};
