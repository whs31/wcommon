#pragma once

#include <optional>
#include <string>
#include <variant>

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

  inline auto constexpr angle_reading = QuantityReading<Angle, AngleFields, 2, 2> {
    .fields =
      {
        field_unit(&AngleFields::degrees, &Angle::from_degrees),
        field_unit(&AngleFields::radians, &Angle::from_radians),
      },
    .literals =
      {
        literal_unit("deg", &Angle::from_degrees),
        literal_unit("rad", &Angle::from_radians),
      },
    .unknown_suffix = "an angle literal needs one of the unit suffixes deg, rad",
  };

  inline void read_angle(Angle& angle, AngleFields const& fields, glz::context& ctx) noexcept
  {
    if(not fields.angle.has_value()) {
      if(fields.units.has_value())
        reject(ctx, "angle units without an angle");
      else
        read_fields(angle, ctx, fields, angle_reading.fields);
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

  inline void read_angle(
    Angle& angle,
    QuantityInput<AngleFields> const& input,
    glz::context& ctx
  ) noexcept
  {
    if(auto const* const literal = std::get_if<std::string>(&input.value()))
      read_literal(angle, ctx, *literal, angle_reading.literals, angle_reading.unknown_suffix);
    else if(auto const* const fields = std::get_if<AngleFields>(&input.value()))
      read_angle(angle, *fields, ctx);
  }
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of Angle: `{"angle": degrees, "units": "degrees"}`.
 * @details Reading also accepts `"units": "radians"`, and the shorter `{"degrees": magnitude}` or
 * `{"radians": magnitude}`. A text format also reads a literal string with the suffix of a C++
 * literal, `"30deg"` or `"0.5rad"`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<radar::mms::common::Angle>
{
  static auto constexpr value = glz::custom<
    [](
      radar::mms::common::Angle& angle,
      radar::mms::common::_glaze_impl::QuantityInput<
        radar::mms::common::_glaze_impl::AngleFields> const& input,
      glz::context& ctx
    ) { radar::mms::common::_glaze_impl::read_angle(angle, input, ctx); },
    [](radar::mms::common::Angle const& angle) {
      return radar::mms::common::_glaze_impl::AngleFields {
        .angle = angle.degrees(),
        .units = "degrees",
      };
    }>;
};
