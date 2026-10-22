#pragma once

#include <radar/mms/common/config.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt glaze adapters require rcommon to be built with with_qt=True"
#endif

#include <qnamespace.h>

#include <radar/mms/common/glaze/detail.h>

// `Qt::MouseButton` is a plain enum, which glaze reads and writes as its integer value without any
// metadata, matching the nlohmann adapter.

/**
 * @brief Glaze metadata of `Qt::MouseButtons`: its integer flags.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<Qt::MouseButtons>
{
  static auto constexpr value = glz::custom<
    [](Qt::MouseButtons& value, int const number) { value = Qt::MouseButtons::fromInt(number); },
    [](Qt::MouseButtons const value) { return static_cast<int>(value); }>;
};
