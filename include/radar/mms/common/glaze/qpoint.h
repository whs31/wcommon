#pragma once

#include <radar/mms/common/config.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt glaze adapters require rcommon to be built with with_qt=True"
#endif

#include <array>

#include <qpoint.h>

#include <radar/mms/common/glaze/detail.h>

/**
 * @brief Glaze metadata of `QPoint`: a two-element array `[x, y]`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<QPoint>
{
  static constexpr auto value = glz::custom<
    [](QPoint& value, std::array<int, 2> const& pair) {
      auto const [first, second] = pair;
      value = QPoint(first, second);
    },
    [](QPoint const& value) { return std::array<int, 2> {value.x(), value.y()}; }>;
};

/**
 * @brief Glaze metadata of `QPointF`: a two-element array `[x, y]`.
 * @ingroup core_glaze
 */
template <>
struct glz::meta<QPointF>
{
  static constexpr auto value = glz::custom<
    [](QPointF& value, std::array<double, 2> const& pair) {
      auto const [first, second] = pair;
      value = QPointF(first, second);
    },
    [](QPointF const& value) { return std::array<double, 2> {value.x(), value.y()}; }>;
};
