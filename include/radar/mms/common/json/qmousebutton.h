#pragma once

#include <radar/mms/common/config.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt JSON adapters require rcommon to be built with with_qt=True"
#endif

#include <qnamespace.h>

#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<Qt::MouseButton>
{
  static void to_json(json& value, Qt::MouseButton const button)
  {
    value = static_cast<int>(button);
  }

  static void from_json(json const& value, Qt::MouseButton& button)
  {
    button = static_cast<Qt::MouseButton>(value.get<int>());
  }
};

template <>
struct adl_serializer<Qt::MouseButtons>
{
  static void to_json(json& value, Qt::MouseButtons const buttons)
  {
    value = static_cast<int>(buttons);
  }

  static void from_json(json const& value, Qt::MouseButtons& buttons)
  {
    buttons = Qt::MouseButtons(value.get<int>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
