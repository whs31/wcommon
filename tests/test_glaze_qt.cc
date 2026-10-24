#include <whs31/common/glaze/qgeocoordinate.h>
#include <whs31/common/glaze/qkey.h>
#include <whs31/common/glaze/qkeymodifier.h>
#include <whs31/common/glaze/qmousebutton.h>
#include <whs31/common/glaze/qmouseeventsource.h>
#include <whs31/common/glaze/qpoint.h>
#include <whs31/common/glaze/qsize.h>

#ifndef WHS31_COMMON_WITH_QT
#  error "Qt configuration must enable WHS31_COMMON_WITH_QT"
#endif

#include <string>

#include <qgeocoordinate.h>
#include <qpoint.h>
#include <qsize.h>

#include <catch2/catch_test_macros.hpp>

#include "glaze_support.h"

using namespace glaze_support;

TEST_CASE("Qt coordinates glaze adapter writes lat, lon and alt", "[glaze][qt][coordinate]")
{
  check_round_trip(
    QGeoCoordinate(55.7558, 37.6173, 156.5),
    R"({"lat":55.7558,"lon":37.6173,"alt":156.5})"
  );
  REQUIRE(read<QGeoCoordinate>(R"({"lat":10,"lon":20})") == QGeoCoordinate(10.0, 20.0, 0.0));
  REQUIRE(write(QGeoCoordinate(10.0, 20.0)) == R"({"lat":10,"lon":20})");
  REQUIRE(read_error<QGeoCoordinate>(R"({"lat":"10","lon":20})") != glz::error_code::none);
  REQUIRE(strict_read_error<QGeoCoordinate>(R"({"lat":10})") == glz::error_code::missing_key);
  REQUIRE(strict_read_error<QGeoCoordinate>(R"({"lat":10,"lon":20})") == glz::error_code::none);
}

TEST_CASE("Qt input enums glaze adapters use integer values", "[glaze][qt][input]")
{
  auto const key = Qt::Key_Enter;
  auto const modifiers = Qt::ControlModifier | Qt::ShiftModifier;
  auto const buttons = Qt::LeftButton | Qt::RightButton;

  check_round_trip(key, std::to_string(static_cast<int>(key)));
  check_round_trip(modifiers, std::to_string(static_cast<int>(modifiers)));
  check_round_trip(Qt::MiddleButton, std::to_string(static_cast<int>(Qt::MiddleButton)));
  check_round_trip(buttons, std::to_string(static_cast<int>(buttons)));
  check_round_trip(
    Qt::MouseEventSynthesizedByApplication,
    std::to_string(static_cast<int>(Qt::MouseEventSynthesizedByApplication))
  );
  REQUIRE(read_error<Qt::Key>(R"("enter")") != glz::error_code::none);
}

TEST_CASE("Qt points and sizes glaze adapters use two-element arrays", "[glaze][qt]")
{
  check_round_trip(QPoint(12, -34), "[12,-34]");
  check_round_trip(QPointF(12.5, -34.25), "[12.5,-34.25]");
  check_round_trip(QSize(640, 480), "[640,480]");
  check_round_trip(QSizeF(640.5, 480.25), "[640.5,480.25]");

  REQUIRE(read_error<QPoint>("[12,-34,5]") != glz::error_code::none);
  REQUIRE(read_error<QPointF>(R"({"x":12,"y":-34})") != glz::error_code::none);
}
