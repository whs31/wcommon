#pragma once

#include <radar/mms/common/config.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt JSON adapters require rcommon to be built with with_qt=True"
#endif

#include <qgeocoordinate.h>

#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<QGeoCoordinate>
{
  static void to_json(json& value, QGeoCoordinate const& coordinate)
  {
    value = {
      {"lat", coordinate.latitude()},
      {"lon", coordinate.longitude()},
      {"alt", coordinate.altitude()},
    };
  }

  static void from_json(json const& value, QGeoCoordinate& coordinate)
  {
    coordinate = QGeoCoordinate(
      value.at("lat").get<double>(),
      value.at("lon").get<double>(),
      value.contains("alt") and value.at("alt").is_number() ? value.at("alt").get<double>() : 0.0
    );
  }
};

NLOHMANN_JSON_NAMESPACE_END
