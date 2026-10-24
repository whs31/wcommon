#pragma once

#include <whs31/common/config.h>

#ifndef WHS31_COMMON_WITH_QT
#  error "Qt JSON adapters require wcommon to be built with with_qt=True"
#endif

#include <qnamespace.h>

#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<Qt::Key>
{
  static void to_json(json& value, Qt::Key const key) { value = static_cast<int>(key); }

  static void from_json(json const& value, Qt::Key& key)
  {
    key = static_cast<Qt::Key>(value.get<int>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
