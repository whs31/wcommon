#include <whs31/common/geodesy.h>

#include <cmath>

#include <memory>
#include <ostream>
#include <string_view>
#include <utility>

#include <whs31/common/bit.h>
#include <whs31/common/check.h>
#include <whs31/common/error.h>
#include <whs31/common/memo.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace
{
  // bowring's iteration converges cubically; three steps reach rounding level near the surface
  auto constexpr latitude_iterations = 3;

  [[nodiscard]] Vector3 ecef_of(GeodeticPosition const& position) noexcept
  {
    auto const sin_latitude = position.latitude.sin();
    auto const prime_vertical_radius = wgs84::semi_major_axis.as_m()
                                     / std::sqrt(
                                         1.0
                                         - wgs84::eccentricity_squared * sin_latitude * sin_latitude
                                     );
    auto const altitude = position.altitude_msl.as_m();
    auto const equatorial = (prime_vertical_radius + altitude) * position.latitude.cos();
    return {
      equatorial * position.longitude.cos(),
      equatorial * position.longitude.sin(),
      (prime_vertical_radius * (1.0 - wgs84::eccentricity_squared) + altitude) * sin_latitude
    };
  }

  // rows north, east and down of the local frame at a latitude and longitude
  [[nodiscard]] Matrix3x3 ecef_to_ned(Angle const latitude, Angle const longitude) noexcept
  {
    auto const sin_latitude = latitude.sin();
    auto const cos_latitude = latitude.cos();
    auto const sin_longitude = longitude.sin();
    auto const cos_longitude = longitude.cos();
    return Matrix3x3::from_rows(
      {-sin_latitude * cos_longitude, -sin_latitude * sin_longitude, cos_latitude},
      {-sin_longitude, cos_longitude, 0.0},
      {-cos_latitude * cos_longitude, -cos_latitude * sin_longitude, -sin_latitude}
    );
  }

  // latitude and longitude of the ellipsoid normal through a point, by bowring's iteration on the
  // parametric latitude, which stays well-conditioned at the poles
  [[nodiscard]] std::pair<Angle, Angle> normal_through(Vector3 const& ecef) noexcept
  {
    // note: [whs31 : 2026-10-14]
    // a simulation asks for it on every terrain query, so the iteration carries the latitude as
    // the direction (north, equatorial) instead of an angle: tan(parametric) = (1 - f)
    // tan(latitude) then needs one square root where the angles took two atan2, a sin and a cos.
    // only the final latitude is an angle
    auto const equatorial = std::sqrt((ecef.x() * ecef.x()) + (ecef.y() * ecef.y()));
    auto north = ecef.z();
    auto across = equatorial * (1.0 - wgs84::eccentricity_squared);
    for(auto iteration = 0; iteration < latitude_iterations; ++iteration) {
      auto const parametric_north = (1.0 - wgs84::flattening) * north;
      auto const length = std::sqrt((parametric_north * parametric_north) + (across * across));
      auto const sin_parametric = parametric_north / length;
      auto const cos_parametric = across / length;
      north = ecef.z()
            + wgs84::second_eccentricity_squared * wgs84::semi_minor_axis.as_m() * sin_parametric
                * sin_parametric * sin_parametric;
      across = equatorial
             - wgs84::eccentricity_squared * wgs84::semi_major_axis.as_m() * cos_parametric
                 * cos_parametric * cos_parametric;
    }
    return {
      Angle::from_radians(std::atan2(north, across)),
      Angle::from_radians(std::atan2(ecef.y(), ecef.x())),
    };
  }

  // note: [whs31 : 2026-10-19]
  // callers look the same points up over and over, e.g. the systems of a simulation step: the
  // environment, the rotors and the contacts at the state the step ends at, again at the start of
  // the next one, a vertical ray at every bisection. keyed by the origin of the plane in ecef and
  // the horizontal position, all the latitude and longitude depend on
  using NormalMemo = Memo<5, std::pair<Angle, Angle>, 256>;

  [[nodiscard]] NormalMemo& normal_memo()
  {
    thread_local auto const memo = std::make_unique<NormalMemo>();
    return *memo;
  }

  void require_finite(GeodeticPosition const& position, std::string_view const what)
  {
    WHS31_HARD_CHECK(  // NOLINT(*-avoid-do-while)
      position.is_finite(),
      InvalidArgumentError,
      "{} must be finite, got {}",
      what,
      position
    );
  }
}  // namespace

std::string GeodeticPosition::to_string() const
{
  return fmt::format(
    "{} deg, {} deg, {} m MSL",
    this->latitude.degrees(),
    this->longitude.degrees(),
    this->altitude_msl.as_m()
  );
}

#ifdef WHS31_COMMON_WITH_QT
QString GeodeticPosition::to_qstring() const { return QString::fromStdString(this->to_string()); }
#endif

std::ostream& operator<<(std::ostream& os, GeodeticPosition const& position)
{
  return os << position.to_string();
}

LocalTangentPlane::LocalTangentPlane()
  : LocalTangentPlane(GeodeticPosition())
{}

LocalTangentPlane::LocalTangentPlane(GeodeticPosition const& origin)
  : m_origin(origin)
{
  require_finite(origin, "tangent plane origin");
  // the macro negates the whole conjunction, which clang-tidy would distribute
  // NOLINTNEXTLINE(*-avoid-do-while,readability-simplify-boolean-expr)
  WHS31_HARD_CHECK(
    std::abs(origin.latitude.degrees()) < 90.0 and std::abs(origin.longitude.degrees()) <= 180.0,
    InvalidArgumentError,
    "tangent plane origin must lie between the poles with its longitude in [-180, 180] deg, got "
    "{} deg, {} deg",
    origin.latitude.degrees(),
    origin.longitude.degrees()
  );
  this->m_origin_ecef = ecef_of(origin);
  this->m_ecef_to_ned = ecef_to_ned(origin.latitude, origin.longitude);
}

GeodeticPosition LocalTangentPlane::lla_from_ned(Vector3 const& position_ned) const
{
  WHS31_HARD_CHECK(  // NOLINT(*-avoid-do-while)
    position_ned.is_finite(),
    InvalidArgumentError,
    "tangent plane position must be finite, got {} m",
    position_ned
  );
  // note: [whs31 : 2026-10-09]
  // the vertical lines of the plane keep their latitude and longitude, so only the horizontal
  // position goes onto it
  auto const key = NormalMemo::Key {
    bits_of(this->m_origin_ecef.x()),
    bits_of(this->m_origin_ecef.y()),
    bits_of(this->m_origin_ecef.z()),
    bits_of(position_ned.x()),
    bits_of(position_ned.y()),
  };
  auto& memo = normal_memo();
  auto const* const kept = memo.find(key);
  auto const horizontal = Vector3 {position_ned.x(), position_ned.y(), 0.0};
  auto const [latitude, longitude] = kept != nullptr
                                     ? *kept
                                     : memo.store(
                                         key,
                                         normal_through(
                                           this->m_origin_ecef
                                           + this->m_ecef_to_ned.transposed() * horizontal
                                         )
                                       );
  return {
    .latitude = latitude,
    .longitude = longitude,
    .altitude_msl = this->altitude_msl(position_ned),
  };
}

Vector3 LocalTangentPlane::ned_from_lla(GeodeticPosition const& position) const
{
  require_finite(position, "geodetic position");
  WHS31_HARD_CHECK(  // NOLINT(*-avoid-do-while)
    std::abs(position.latitude.degrees()) <= 90.0,
    InvalidArgumentError,
    "latitude must be in [-90, 90] deg, got {} deg",
    position.latitude.degrees()
  );
  // the normal through the position, followed from the origin's altitude to the plane
  auto const surface = ecef_of({
    .latitude = position.latitude,
    .longitude = position.longitude,
    .altitude_msl = this->m_origin.altitude_msl,
  });
  auto const up = -ecef_to_ned(position.latitude, position.longitude).row(2);
  auto const origin_up = -this->m_ecef_to_ned.row(2);
  auto const alignment = dot(up, origin_up);
  WHS31_HARD_CHECK(  // NOLINT(*-avoid-do-while)
    alignment > 0.0,
    NumericError,
    "{} deg, {} deg is too far from the tangent plane origin {} deg, {} deg to map onto the plane",
    position.latitude.degrees(),
    position.longitude.degrees(),
    this->m_origin.latitude.degrees(),
    this->m_origin.longitude.degrees()
  );
  auto const rise = -dot(surface - this->m_origin_ecef, origin_up) / alignment;
  auto const horizontal = this->m_ecef_to_ned * (surface + up * rise - this->m_origin_ecef);
  return {
    horizontal.x(),
    horizontal.y(),
    (this->m_origin.altitude_msl - position.altitude_msl).as_m()
  };
}

WHS31_COMMON_NAMESPACE_END
