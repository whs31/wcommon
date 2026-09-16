#include <radar/mms/common/json/qgeocoordinate.h>
#include <radar/mms/common/json/qkey.h>
#include <radar/mms/common/json/qkeymodifier.h>
#include <radar/mms/common/json/qmousebutton.h>
#include <radar/mms/common/json/qmouseeventsource.h>
#include <radar/mms/common/json/qpoint.h>
#include <radar/mms/common/json/qsize.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt configuration must enable RCOMMON_WITH_QT"
#endif

#include <qevent.h>
#include <qgeocoordinate.h>
#include <qpoint.h>
#include <qsize.h>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Qt coordinates serialize to lossless JSON objects", "[json][qt][coordinate]")
{
  auto const coordinate = QGeoCoordinate(55.7558, 37.6173, 156.5);
  auto const json = nlohmann::json(coordinate);

  REQUIRE(json == nlohmann::json({{"lat", 55.7558}, {"lon", 37.6173}, {"alt", 156.5}}));
  CHECK(json.get<QGeoCoordinate>() == coordinate);

  auto const without_altitude = nlohmann::json({{"lat", 10.0}, {"lon", 20.0}})
                                  .get<QGeoCoordinate>();
  CHECK(without_altitude == QGeoCoordinate(10.0, 20.0, 0.0));
  CHECK_THROWS(nlohmann::json({{"lat", "10"}, {"lon", 20.0}}).get<QGeoCoordinate>());
  CHECK_THROWS(nlohmann::json({{"lat", 10.0}}).get<QGeoCoordinate>());
}

TEST_CASE("Qt input enums serialize as their integer values", "[json][qt][input]")
{
  auto const key = Qt::Key_Enter;
  auto const modifiers = Qt::ControlModifier | Qt::ShiftModifier;
  auto const button = Qt::MiddleButton;
  auto const buttons = Qt::LeftButton | Qt::RightButton;
  auto const source = Qt::MouseEventSynthesizedByApplication;

  CHECK(nlohmann::json(key) == static_cast<int>(key));
  CHECK(nlohmann::json(modifiers) == static_cast<int>(modifiers));
  CHECK(nlohmann::json(button) == static_cast<int>(button));
  CHECK(nlohmann::json(buttons) == static_cast<int>(buttons));
  CHECK(nlohmann::json(source) == static_cast<int>(source));

  CHECK(nlohmann::json(static_cast<int>(key)).get<Qt::Key>() == key);
  CHECK(nlohmann::json(static_cast<int>(modifiers)).get<Qt::KeyboardModifiers>() == modifiers);
  CHECK(nlohmann::json(static_cast<int>(button)).get<Qt::MouseButton>() == button);
  CHECK(nlohmann::json(static_cast<int>(buttons)).get<Qt::MouseButtons>() == buttons);
  CHECK(nlohmann::json(static_cast<int>(source)).get<Qt::MouseEventSource>() == source);
  CHECK_THROWS(nlohmann::json("enter").get<Qt::Key>());
}

TEST_CASE("Qt points serialize as two-element arrays", "[json][qt][point]")
{
  auto const point = QPoint(12, -34);
  auto const point_f = QPointF(12.5, -34.25);

  CHECK(nlohmann::json(point) == nlohmann::json::array({12, -34}));
  CHECK(nlohmann::json(point_f) == nlohmann::json::array({12.5, -34.25}));
  CHECK(nlohmann::json::array({12, -34}).get<QPoint>() == point);
  CHECK(nlohmann::json::array({12.5, -34.25}).get<QPointF>() == point_f);
  CHECK_THROWS(nlohmann::json::array({12}).get<QPoint>());
  CHECK_THROWS(nlohmann::json({{"x", 12}, {"y", -34}}).get<QPointF>());
}

TEST_CASE("Qt sizes serialize as two-element arrays", "[json][qt][size]")
{
  auto const size = QSize(640, 480);
  auto const size_f = QSizeF(640.5, 480.25);

  CHECK(nlohmann::json(size) == nlohmann::json::array({640, 480}));
  CHECK(nlohmann::json(size_f) == nlohmann::json::array({640.5, 480.25}));
  CHECK(nlohmann::json::array({640, 480}).get<QSize>() == size);
  CHECK(nlohmann::json::array({640.5, 480.25}).get<QSizeF>() == size_f);
  CHECK_THROWS(nlohmann::json::array({640}).get<QSize>());
  CHECK_THROWS(nlohmann::json({{"width", 640}, {"height", 480}}).get<QSizeF>());
}
