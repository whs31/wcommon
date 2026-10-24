#pragma once

#include <whs31/common/config.h>

#ifndef WHS31_COMMON_WITH_QT
#  error "Qt JSON adapters require wcommon to be built with with_qt=True"
#endif

#include <qnamespace.h>

#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<Qt::KeyboardModifiers>
{
  static void to_json(json& value, Qt::KeyboardModifiers const modifiers)
  {
    value = static_cast<int>(modifiers);
  }

  static void from_json(json const& value, Qt::KeyboardModifiers& modifiers)
  {
    modifiers = Qt::KeyboardModifiers(value.get<int>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
