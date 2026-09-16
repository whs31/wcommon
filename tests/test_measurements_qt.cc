#include <radar/mms/common/angle.h>
#include <radar/mms/common/distance.h>
#include <radar/mms/common/euler_angles.h>
#include <radar/mms/common/velocity.h>

#include <qmetatype.h>
#include <qstring.h>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms::common;

TEST_CASE(
  "Measurement Qt integration exposes strings, metatypes and properties",
  "[measurements][qt]"
)
{
  REQUIRE(Angle::from_degrees(45.0F).to_qstring() == QString("45 deg"));
  REQUIRE(Distance::from_meters(1'500.0F).to_qstring() == QString("1.5 km"));
  REQUIRE(Velocity::from_mps(25.0F).to_qstring() == QString("25 m/s"));
  REQUIRE(EulerAngles::from_degrees(1.0F, 2.0F, 3.0F).to_qstring().contains("yaw 3 deg"));

  REQUIRE(QMetaType::fromType<Angle>().isValid());
  REQUIRE(QMetaType::fromType<Distance>().isValid());
  REQUIRE(QMetaType::fromType<Velocity>().isValid());
  REQUIRE(QMetaType::fromType<EulerAngles>().isValid());
  REQUIRE(Angle::staticMetaObject.indexOfProperty("degrees") >= 0);
  REQUIRE(Distance::staticMetaObject.indexOfProperty("kilometers") >= 0);
  REQUIRE(Velocity::staticMetaObject.indexOfProperty("kilometersPerHour") >= 0);
  REQUIRE(EulerAngles::staticMetaObject.indexOfProperty("yawDegrees") >= 0);
}
