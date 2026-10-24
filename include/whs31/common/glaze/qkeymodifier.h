#pragma once

#include <whs31/common/config.h>

#ifndef WHS31_COMMON_WITH_QT
#  error "Qt glaze adapters require wcommon to be built with with_qt=True"
#endif

#include <qnamespace.h>

#include <whs31/common/glaze/detail.h>

/**
 * @brief Glaze metadata of `Qt::KeyboardModifiers`: its integer flags.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<Qt::KeyboardModifiers>
{
  static auto constexpr value = glz::custom<
    [](Qt::KeyboardModifiers& value, int const number) {
      value = Qt::KeyboardModifiers::fromInt(number);
    },
    [](Qt::KeyboardModifiers const value) { return static_cast<int>(value); }>;
};
