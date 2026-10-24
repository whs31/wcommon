#pragma once

#include <whs31/common/config.h>

#ifndef WHS31_COMMON_WITH_QT
#  error "Qt JSON adapters require wcommon to be built with with_qt=True"
#endif

#include <qpoint.h>

#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<QPoint>
{
  static void to_json(json& value, QPoint const& point)
  {
    value = json::array({point.x(), point.y()});
  }

  static void from_json(json const& value, QPoint& point)
  {
    if(not value.is_array() or value.size() != 2)
      throw json::type_error::create(302, "Invalid QPoint JSON array", nullptr);
    point = QPoint(value.at(0).get<int>(), value.at(1).get<int>());
  }
};

template <>
struct adl_serializer<QPointF>
{
  static void to_json(json& value, QPointF const& point)
  {
    value = json::array({point.x(), point.y()});
  }

  static void from_json(json const& value, QPointF& point)
  {
    if(not value.is_array() or value.size() != 2)
      throw json::type_error::create(302, "Invalid QPointF JSON array", nullptr);
    point = QPointF(value.at(0).get<double>(), value.at(1).get<double>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
