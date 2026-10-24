#pragma once

#include <whs31/common/config.h>

#ifndef WHS31_COMMON_WITH_QT
#  error "Qt JSON adapters require wcommon to be built with with_qt=True"
#endif

#include <qnamespace.h>

#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<Qt::MouseEventSource>
{
  static void to_json(json& value, Qt::MouseEventSource const source)
  {
    value = static_cast<int>(source);
  }

  static void from_json(json const& value, Qt::MouseEventSource& source)
  {
    source = static_cast<Qt::MouseEventSource>(value.get<int>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
