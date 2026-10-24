#include <whs31/common/all.h>

#include <qmetatype.h>
#include <qstring.h>
#include <qvariant.h>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace whs31;

TEMPLATE_TEST_CASE(
  "Quantities are Qt gadgets with string and finite properties",
  "[quantities][qt]",
  Mass,
  Force,
  Torque,
  Pressure,
  Temperature,
  Density,
  AngularVelocity,
  AngularAcceleration,
  Acceleration,
  Frequency,
  MomentOfInertia,
  Voltage,
  Current,
  Resistance,
  Conductance,
  Power,
  Energy,
  Charge,
  Capacitance,
  MagneticFluxDensity
)
{
  using T = TestType;

  auto const value = T(2.0);
  REQUIRE(QMetaType::fromType<T>().isValid());
  REQUIRE(value.to_qstring() == QString::fromStdString(value.to_string()));

  auto const& meta = T::staticMetaObject;
  auto const finite = meta.property(meta.indexOfProperty("isFinite"));
  REQUIRE(finite.isValid());
  REQUIRE(finite.readOnGadget(&value).toBool());

  auto const string = meta.property(meta.indexOfProperty("string"));
  REQUIRE(string.readOnGadget(&value).toString() == value.to_qstring());
}

TEST_CASE("Quantity gadgets expose their units as properties", "[quantities][qt]")
{
  auto const read = [](auto const& value, char const* name) {
    auto const& meta = std::remove_cvref_t<decltype(value)>::staticMetaObject;
    auto const property = meta.property(meta.indexOfProperty(name));
    REQUIRE(property.isValid());
    return property.readOnGadget(&value).toDouble();
  };

  REQUIRE(read(Mass::from_kilograms(1.5), "grams") == 1'500.0);
  REQUIRE(read(Pressure::from_hectopascals(1'013.25), "pascals") == 101'325.0);
  REQUIRE(read(Temperature::from_kelvin(273.15), "celsius") == 0.0);
  REQUIRE(approx_eq(
    read(AngularVelocity::from_degrees_per_second(360.0), "revolutionsPerMinute"),
    60.0,
    1e-9
  ));
  REQUIRE(approx_eq(read(Charge::from_ampere_hours(2.0), "milliampereHours"), 2'000.0, 1e-9));
  REQUIRE(read(Distance::from_meters(2.0), "meters") == 2.0);
  REQUIRE(read(Distance::from_meters(2.0), "isFinite") == 1.0);
}
