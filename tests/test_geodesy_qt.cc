#include <whs31/common/geodesy.h>

#include <qmetatype.h>
#include <qstring.h>
#include <qvariant.h>

#include <catch2/catch_test_macros.hpp>

using namespace whs31::common;

TEST_CASE("Geodetic positions are Qt gadgets with plain-number properties", "[geodesy][qt]")
{
  auto position = GeodeticPosition {
    .latitude = 59.5_deg,
    .longitude = -30.25_deg,
    .altitude_msl = 70_meters,
  };
  REQUIRE(QMetaType::fromType<GeodeticPosition>().isValid());
  REQUIRE(position.to_qstring() == QString("59.5 deg, -30.25 deg, 70 m MSL"));

  auto const& meta = GeodeticPosition::staticMetaObject;
  auto const read = [&](char const* const name) {
    auto const property = meta.property(meta.indexOfProperty(name));
    REQUIRE(property.isValid());
    return property.readOnGadget(&position);
  };
  REQUIRE(read("latitudeDegrees").toDouble() == 59.5);
  REQUIRE(read("longitudeDegrees").toDouble() == -30.25);
  REQUIRE(read("altitudeMslMeters").toDouble() == 70.0);
  REQUIRE(read("latitude").value<Angle>() == 59.5_deg);
  REQUIRE(read("string").toString() == position.to_qstring());

  auto const altitude = meta.property(meta.indexOfProperty("altitudeMsl"));
  REQUIRE(altitude.writeOnGadget(&position, QVariant::fromValue(120_meters)));
  REQUIRE(position.altitude_msl == 120_meters);
}
