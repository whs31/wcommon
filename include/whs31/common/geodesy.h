#pragma once

#include <iosfwd>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <whs31/common/angle.h>
#include <whs31/common/config.h>
#include <whs31/common/distance.h>
#include <whs31/common/matrix3x3.h>
#include <whs31/common/vector3.h>
#include <whs31/common/detail/export.h>
#include <whs31/common/detail/namespace.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qmetatype.h>
#  include <qobjectdefs.h>
#  include <qstring.h>
#endif

/**
 * @defgroup core_geodesy Geodesy
 * @ingroup core
 * @brief Positions on the WGS84 ellipsoid and the local frames tangent to it.
 */

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief The WGS84 ellipsoid.
 * @ingroup core_geodesy
 */
namespace wgs84
{
  /// @brief Equatorial radius.
  inline auto constexpr semi_major_axis = 6'378'137_meters;

  /// @brief Flattening `f = (a − b) / a`.
  inline auto constexpr flattening = 1.0 / 298.257'223'563;

  /// @brief Polar radius `b = a (1 − f)`.
  inline auto constexpr semi_minor_axis = semi_major_axis * (1.0 - flattening);

  /// @brief First eccentricity squared, `e² = f (2 − f)`.
  inline auto constexpr eccentricity_squared = flattening * (2.0 - flattening);

  /// @brief Second eccentricity squared, `e'² = e² / (1 − e²)`.
  inline auto constexpr second_eccentricity_squared = eccentricity_squared
                                                    / (1.0 - eccentricity_squared);
}  // namespace wgs84

/**
 * @brief WGS84 latitude and longitude with the altitude above mean sea level, positive up.
 * @details An aggregate, so that it is built with designated initializers and read from
 * configuration as a table of its three members:
 *
 * @code {.cpp}
 * auto constexpr pulkovo = whs31::GeodeticPosition {
 *   .latitude = 59.771_deg,
 *   .longitude = 30.326_deg,
 *   .altitude_msl = 70_meters,
 * };
 * @endcode
 *
 * Latitudes are positive north, longitudes positive east.
 *
 * There is no `glz::meta` for it: glaze reflects the aggregate, and with the adapters of Angle and
 * Distance (`glaze/angle.h`, `glaze/distance.h`) writes the JSON of `json/geodesy.h`. Code built
 * on glaze reflection of aggregates keeps working with it.
 * @note Qt properties and metatype registration are available only when wcommon is built with
 * `with_qt=True`; QML reads the plain numbers through `latitudeDegrees`, `longitudeDegrees` and
 * `altitudeMslMeters`.
 * @ingroup core_geodesy
 * @see LocalTangentPlane
 */
struct WHS31_COMMON_API GeodeticPosition
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET
  Q_PROPERTY(Angle latitude MEMBER latitude FINAL)
  Q_PROPERTY(Angle longitude MEMBER longitude FINAL)
  Q_PROPERTY(Distance altitudeMsl MEMBER altitude_msl FINAL)
  Q_PROPERTY(double latitudeDegrees READ latitude_degrees CONSTANT FINAL)
  Q_PROPERTY(double longitudeDegrees READ longitude_degrees CONSTANT FINAL)
  Q_PROPERTY(double altitudeMslMeters READ altitude_msl_meters CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)

 public:
#endif
  Angle latitude;
  Angle longitude;
  Distance altitude_msl;

  [[nodiscard]] constexpr double latitude_degrees() const noexcept
  {
    return this->latitude.degrees();
  }

  [[nodiscard]] constexpr double longitude_degrees() const noexcept
  {
    return this->longitude.degrees();
  }

  [[nodiscard]] constexpr double altitude_msl_meters() const noexcept
  {
    return this->altitude_msl.as_m();
  }

  [[nodiscard]] constexpr bool is_finite() const noexcept
  {
    return this->latitude.is_finite() and this->longitude.is_finite()
       and this->altitude_msl.is_finite();
  }

  /// @brief `<latitude> deg, <longitude> deg, <altitude> m MSL`, at full precision.
  [[nodiscard]] std::string to_string() const;

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] constexpr bool operator==(GeodeticPosition const&) const noexcept = default;
};

WHS31_COMMON_API std::ostream& operator<<(std::ostream& os, GeodeticPosition const& position);

/**
 * @brief A local frame on the Earth: the plane tangent to the WGS84 ellipsoid at an origin, x
 * north, y east, z down along the normal of the origin (NED).
 * @details Horizontally the geometry is exact: a point of the plane has the latitude and
 * longitude of the ellipsoid normal passing through it. Against WGS84 geodesics from the origin,
 * the horizontal error grows as s³/3R²: 8 mm at 10 km, 1 m at 50 km, 8 m at 100 km, at any
 * latitude.
 *
 * Vertically the frame is flat: every vertical line of the plane keeps its latitude and
 * longitude, and the altitude is `origin.altitude_msl − z` everywhere. The plane rises above the
 * ellipsoid away from the origin (by s²/2R, 7.8 m at 10 km), which this ignores, so that
 * altitudes, terrain heights and a constant gravity along z agree with each other. The plane
 * passes through the origin at its altitude, so horizontal distances scale with the radius at
 * that altitude. Altitudes above mean sea level stand in for heights above the ellipsoid, which
 * only moves the plane by the geoid undulation (tens of metres, 10⁻⁵ of the scale).
 *
 * lla_from_ned() keeps the latitude and longitude of the positions it was asked for last, per
 * thread, so asking again for the same horizontal position is a lookup.
 *
 * @code {.cpp}
 * using namespace whs31;
 *
 * auto const plane = LocalTangentPlane({.latitude = 60.2_deg, .longitude = 30.3_deg});
 * auto const position = plane.lla_from_ned({10'000.0, 0.0, -150.0});  // 10 km north, 150 m up
 * auto const back = plane.ned_from_lla(position);
 * @endcode
 *
 * @ingroup core_geodesy
 * @see GeodeticPosition
 */
class WHS31_COMMON_API LocalTangentPlane
{
 public:
  /// @brief The plane at latitude, longitude and altitude zero.
  LocalTangentPlane();

  /**
   * @throws InvalidArgumentError If the origin is not finite, its latitude is not strictly between
   * the poles or its longitude is outside [-180°, 180°].
   */
  explicit LocalTangentPlane(GeodeticPosition const& origin);

  [[nodiscard]] GeodeticPosition const& origin() const noexcept { return this->m_origin; }

  /**
   * @brief Latitude, longitude in (-180°, 180°] and altitude of a position in the plane's frame.
   * @param position_ned Metres north, east and down of the origin.
   * @throws InvalidArgumentError If the position is not finite.
   */
  [[nodiscard]] GeodeticPosition lla_from_ned(Vector3 const& position_ned) const;

  /**
   * @brief Position in the plane's frame of a geodetic position, the inverse of lla_from_ned().
   * @return Metres north, east and down of the origin.
   * @throws InvalidArgumentError If the position is not finite or its latitude is outside
   * [-90°, 90°].
   * @throws NumericError If the position is a quarter of the globe or more away from the origin,
   * where its normal never meets the plane.
   */
  [[nodiscard]] Vector3 ned_from_lla(GeodeticPosition const& position) const;

  /// @brief Altitude above mean sea level of a position in the plane's frame.
  [[nodiscard]] Distance altitude_msl(Vector3 const& position_ned) const noexcept
  {
    return this->m_origin.altitude_msl - Distance::from_meters(position_ned.z());
  }

 private:
  GeodeticPosition m_origin;
  // the origin and the rows north, east, down of the plane, in earth-centred earth-fixed axes
  Vector3 m_origin_ecef;
  Matrix3x3 m_ecef_to_ned;
};

WHS31_COMMON_NAMESPACE_END

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<whs31::common::GeodeticPosition> : fmt::ostream_formatter
{};
#endif

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::GeodeticPosition)
#endif
