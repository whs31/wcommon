#pragma once

#include <radar/mms/common/config.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt JSON adapters require rcommon to be built with with_qt=True"
#endif

#include <qsize.h>

#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<QSize>
{
  static void to_json(json& value, QSize const& size)
  {
    value = json::array({size.width(), size.height()});
  }

  static void from_json(json const& value, QSize& size)
  {
    if(not value.is_array() or value.size() != 2)
      throw json::type_error::create(302, "Invalid QSize JSON array", nullptr);
    size = QSize(value.at(0).get<int>(), value.at(1).get<int>());
  }
};

template <>
struct adl_serializer<QSizeF>
{
  static void to_json(json& value, QSizeF const& size)
  {
    value = json::array({size.width(), size.height()});
  }

  static void from_json(json const& value, QSizeF& size)
  {
    if(not value.is_array() or value.size() != 2)
      throw json::type_error::create(302, "Invalid QSizeF JSON array", nullptr);
    size = QSizeF(value.at(0).get<double>(), value.at(1).get<double>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
