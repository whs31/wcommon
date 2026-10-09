#pragma once

#include <radar/mms/common/config.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt glaze adapters require rcommon to be built with with_qt=True"
#endif

#include <cmath>
#include <optional>

#include <qgeocoordinate.h>

#include <radar/mms/common/glaze/detail.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  struct GeoCoordinateFields
  {
    double lat = 0.0;
    double lon = 0.0;
    std::optional<double> alt;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Glaze metadata of `QGeoCoordinate`: `{"lat": latitude, "lon": longitude, "alt":
 * altitude}`.
 * @details A missing or null altitude reads as zero. An unset (NaN) altitude is not written.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<QGeoCoordinate>
{
  static constexpr auto value = glz::custom<
    [](
      QGeoCoordinate& coordinate,
      radar::mms::common::_glaze_impl::GeoCoordinateFields const& fields
    ) { coordinate = QGeoCoordinate(fields.lat, fields.lon, fields.alt.value_or(0.0)); },
    [](QGeoCoordinate const& coordinate) {
      auto const altitude = coordinate.altitude();
      return radar::mms::common::_glaze_impl::GeoCoordinateFields {
        .lat = coordinate.latitude(),
        .lon = coordinate.longitude(),
        .alt = std::isnan(altitude) ? std::nullopt : std::optional<double>(altitude),
      };
    }>;
};
