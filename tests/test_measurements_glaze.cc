#include <radar/mms/common/glaze/acceleration.h>
#include <radar/mms/common/glaze/angle.h>
#include <radar/mms/common/glaze/angular_acceleration.h>
#include <radar/mms/common/glaze/angular_velocity.h>
#include <radar/mms/common/glaze/capacitance.h>
#include <radar/mms/common/glaze/charge.h>
#include <radar/mms/common/glaze/conductance.h>
#include <radar/mms/common/glaze/current.h>
#include <radar/mms/common/glaze/density.h>
#include <radar/mms/common/glaze/distance.h>
#include <radar/mms/common/glaze/energy.h>
#include <radar/mms/common/glaze/euler_angles.h>
#include <radar/mms/common/glaze/force.h>
#include <radar/mms/common/glaze/frequency.h>
#include <radar/mms/common/glaze/magnetic_flux_density.h>
#include <radar/mms/common/glaze/mass.h>
#include <radar/mms/common/glaze/moment_of_inertia.h>
#include <radar/mms/common/glaze/power.h>
#include <radar/mms/common/glaze/pressure.h>
#include <radar/mms/common/glaze/resistance.h>
#include <radar/mms/common/glaze/temperature.h>
#include <radar/mms/common/glaze/torque.h>
#include <radar/mms/common/glaze/velocity.h>
#include <radar/mms/common/glaze/voltage.h>

#include <numbers>
#include <string>
#include <vector>

#include <glaze/beve.hpp>
#include <glaze/json.hpp>
#include <glaze/toml.hpp>

#include <catch2/catch_test_macros.hpp>

#include "glaze_support.h"

using namespace radar::mms;
using namespace glaze_support;
using namespace radar::mms::literals;

namespace
{
  template <typename T>
  void check_quantity(std::string const& key)
  {
    check_round_trip(T(12.5), R"({")" + key + R"(":12.5})");

    REQUIRE(read_error<T>(R"({")" + key + R"(":"12.5"})") != glz::error_code::none);
    REQUIRE(read_error<T>(R"({"value":12.5})") == glz::error_code::unknown_key);
    REQUIRE(strict_read_error<T>(R"({})") != glz::error_code::none);
  }

  // every suffix of the quantity reads the magnitude in its unit
  template <typename T, auto const& Reading>
  void check_literals()
  {
    // NOLINTNEXTLINE(readability-redundant-parentheses): a misread reference template parameter
    for(auto const& unit : Reading.literals) {
      INFO(unit.suffix);
      REQUIRE(read<T>(R"("2.5)" + std::string(unit.suffix) + R"(")") == unit.from(2.5));
    }
  }
}  // namespace

// note: [whs31 : 2026-10-10]
// glaze reflects on the names of the members, which clang rejects for a type without linkage, so
// these stay out of the anonymous namespace
namespace measurements_glaze_test
{
  struct Rotor  // NOLINT(misc-use-internal-linkage)
  {
    AngularVelocity speed;
    Torque load;
    Temperature winding;
  };

  struct Battery  // NOLINT(misc-use-internal-linkage)
  {
    Charge capacity;
    std::vector<Voltage> cutoff;
    Temperature ambient;
  };
}  // namespace measurements_glaze_test

using measurements_glaze_test::Battery;
using measurements_glaze_test::Rotor;

TEST_CASE("Quantity glaze adapters store the SI magnitude under the unit name", "[glaze]")
{
  check_quantity<Distance>("meters");
  check_quantity<Velocity>("mps");
  check_quantity<Mass>("kilograms");
  check_quantity<Force>("newtons");
  check_quantity<Torque>("newton_meters");
  check_quantity<Pressure>("pascals");
  check_quantity<Temperature>("kelvin");
  check_quantity<Density>("kilograms_per_cubic_meter");
  check_quantity<AngularVelocity>("radians_per_second");
  check_quantity<AngularAcceleration>("radians_per_second_squared");
  check_quantity<Acceleration>("meters_per_second_squared");
  check_quantity<Frequency>("hertz");
  check_quantity<MomentOfInertia>("kilogram_square_meters");
  check_quantity<Voltage>("volts");
  check_quantity<Current>("amperes");
  check_quantity<Resistance>("ohms");
  check_quantity<Conductance>("siemens");
  check_quantity<Power>("watts");
  check_quantity<Energy>("joules");
  check_quantity<Charge>("coulombs");
  check_quantity<Capacitance>("farads");
  check_quantity<MagneticFluxDensity>("teslas");
}

TEST_CASE("Quantity glaze adapters read any unit the quantity has a factory for", "[glaze]")
{
  REQUIRE(read<Charge>(R"({"ampere_hours":52})") == Charge::from_ampere_hours(52.0));
  REQUIRE(read<Temperature>(R"({"celsius":15})") == Temperature::from_celsius(15.0));
  REQUIRE(
    read<AngularVelocity>(R"({"degrees_per_second":90})")
    == AngularVelocity::from_degrees_per_second(90.0)
  );
  REQUIRE(read<Distance>(R"({"kilometers":1.5})") == Distance::from_kilometers(1.5));
  REQUIRE(read<Pressure>(R"({"hectopascals":1013.25})") == Pressure::from_hectopascals(1013.25));
  REQUIRE(read<MagneticFluxDensity>(R"({"gauss":0.5})") == MagneticFluxDensity::from_gauss(0.5));

  // the SI unit is still what is written
  REQUIRE(write(Charge::from_ampere_hours(1.0)) == R"({"coulombs":3600})");

  REQUIRE(
    read_error<Charge>(R"({"coulombs":1,"ampere_hours":1})") == glz::error_code::constraint_violated
  );
  REQUIRE(read_error<Charge>(R"({})") == glz::error_code::constraint_violated);
  REQUIRE(read_error<Charge>(R"({"kiloampere_hours":1})") == glz::error_code::unknown_key);
}

TEST_CASE("Quantity glaze adapters read literal strings in text formats", "[glaze][literal]")
{
  // the suffixes are those of the C++ literals
  REQUIRE(read<Frequency>(R"("400hz")") == 400_hz);
  REQUIRE(read<Frequency>(R"("1.5khz")") == 1.5_khz);
  REQUIRE(read<Distance>(R"("-1.5e3m")") == -1.5e3_m);
  REQUIRE(read<Distance>(R"("12mm")") == 12_mm);
  REQUIRE(read<Charge>(R"("52ah")") == 52_ah);
  REQUIRE(read<Temperature>(R"("15degc")") == 15_degc);
  REQUIRE(read<AngularVelocity>(R"("428.6deg_per_s")") == 428.6_deg_per_s);
  REQUIRE(read<MomentOfInertia>(R"("0.15kg_m2")") == 0.15_kg_m2);
  REQUIRE(read<Torque>(R"("2nm")") == 2_nm);
  REQUIRE(read<Angle>(R"("30deg")") == 30_deg);
  REQUIRE(read<Angle>(R"("0.5rad")") == 0.5_rad);

  // the object form is still what is written
  REQUIRE(write(read<Frequency>(R"("1khz")")) == R"({"hertz":1000})");

  using namespace radar::mms::common::_glaze_impl;
  check_literals<Acceleration, acceleration_reading>();
  check_literals<Angle, angle_reading>();
  check_literals<AngularAcceleration, angular_acceleration_reading>();
  check_literals<AngularVelocity, angular_velocity_reading>();
  check_literals<Capacitance, capacitance_reading>();
  check_literals<Charge, charge_reading>();
  check_literals<Conductance, conductance_reading>();
  check_literals<Current, current_reading>();
  check_literals<Density, density_reading>();
  check_literals<Distance, distance_reading>();
  check_literals<Energy, energy_reading>();
  check_literals<Force, force_reading>();
  check_literals<Frequency, frequency_reading>();
  check_literals<MagneticFluxDensity, magnetic_flux_density_reading>();
  check_literals<Mass, mass_reading>();
  check_literals<MomentOfInertia, moment_of_inertia_reading>();
  check_literals<Power, power_reading>();
  check_literals<Pressure, pressure_reading>();
  check_literals<Resistance, resistance_reading>();
  check_literals<Temperature, temperature_reading>();
  check_literals<Torque, torque_reading>();
  check_literals<Velocity, velocity_reading>();
  check_literals<Voltage, voltage_reading>();
}

TEST_CASE("Quantity glaze adapters reject malformed literal strings", "[glaze][literal]")
{
  for(auto const* const literal : {"400", "hz", "400 hz", "400Hz", "400_hz", "400hzz", ""}) {
    INFO(literal);
    REQUIRE(
      read_error<Frequency>(R"(")" + std::string(literal) + R"(")")
      == glz::error_code::constraint_violated
    );
  }
  REQUIRE(read_error<Angle>(R"("30")") == glz::error_code::constraint_violated);

  // the message names the suffixes the quantity takes
  auto value = Frequency {};
  auto const json = std::string(R"("400hertz")");
  auto const error = glz::read_json(value, json);
  REQUIRE(glz::format_error(error, json).contains("unit suffixes hz, khz"));
}

TEST_CASE("Quantity glaze adapters read binary formats as objects only", "[glaze][literal]")
{
  auto buffer = std::string {};
  REQUIRE_FALSE(glz::write_beve(std::string("400hz"), buffer));
  auto value = Frequency {};
  REQUIRE(glz::read_beve(value, buffer));
}

TEST_CASE("Quantity glaze adapters read TOML in natural units", "[glaze][toml]")
{
  auto battery = Battery {};
  auto const toml = std::string(R"(capacity = { ampere_hours = 52.0 }
cutoff = [{ volts = 42.0 }, { millivolts = 58800.0 }]
ambient = { celsius = 15.0 }
)");
  auto const error = glz::read_toml(battery, toml);
  INFO(glz::format_error(error, toml));
  REQUIRE_FALSE(error);
  REQUIRE(battery.capacity == Charge::from_ampere_hours(52.0));
  REQUIRE(battery.cutoff == std::vector {Voltage::from_volts(42.0), Voltage::from_volts(58.8)});
  REQUIRE(battery.ambient == Temperature::from_celsius(15.0));

  auto literals = Battery {};
  auto const literal_toml = std::string(R"(capacity = "52ah"
cutoff = ['42v', { millivolts = 58800.0 }, "-1mv"]
ambient = "15degc"
)");
  auto const literal_error = glz::read_toml(literals, literal_toml);
  INFO(glz::format_error(literal_error, literal_toml));
  REQUIRE_FALSE(literal_error);
  REQUIRE(literals.capacity == 52_ah);
  REQUIRE(literals.cutoff == std::vector {42_v, 58.8_v, -1_mv});
  REQUIRE(literals.ambient == 15_degc);
}

TEST_CASE("Quantity glaze adapters compose into reflected structs", "[glaze]")
{
  auto const rotor = Rotor {
    .speed = AngularVelocity::from_rpm(300.0),
    .load = Torque::from_newton_meters(1.5),
    .winding = Temperature::from_kelvin(300.0),
  };
  auto const json = write(rotor);
  REQUIRE(json.contains(R"("load":{"newton_meters":1.5})"));
  REQUIRE(json.contains(R"("winding":{"kelvin":300})"));

  auto const back = read<Rotor>(json);
  REQUIRE(back.speed == rotor.speed);
  REQUIRE(back.load == rotor.load);
  REQUIRE(back.winding == rotor.winding);

  auto const cells = read<std::vector<Voltage>>(R"([{"volts":3.7},{"volts":4.2}])");
  REQUIRE(cells == std::vector {Voltage::from_volts(3.7), Voltage::from_volts(4.2)});
}

TEST_CASE("Quantity glaze adapters work with binary formats", "[glaze]")
{
  auto const rotor = Rotor {
    .speed = AngularVelocity::from_rpm(300.0),
    .load = Torque::from_newton_meters(1.5),
    .winding = Temperature::from_kelvin(300.0),
  };
  auto buffer = std::string {};
  REQUIRE_FALSE(glz::write_beve(rotor, buffer));

  auto back = Rotor {};
  REQUIRE_FALSE(glz::read_beve(back, buffer));
  REQUIRE(back.speed == rotor.speed);
  REQUIRE(back.load == rotor.load);
  REQUIRE(back.winding == rotor.winding);
}

TEST_CASE("Angle glaze adapter writes degrees and reads degrees or radians", "[glaze][angle]")
{
  check_round_trip(Angle::from_degrees(90.0), R"({"angle":90,"units":"degrees"})");

  auto const pi = read<Angle>(R"({"angle":3.141592653589793,"units":"radians"})");
  REQUIRE(pi == Angle::from_radians(std::numbers::pi_v<double>));

  REQUIRE(
    read_error<Angle>(R"({"angle":90,"units":"turns"})") == glz::error_code::constraint_violated
  );
  REQUIRE(read_error<Angle>(R"({"angle":"90","units":"degrees"})") != glz::error_code::none);
}

TEST_CASE("Angle glaze adapter reads the magnitude under its unit", "[glaze][angle]")
{
  REQUIRE(read<Angle>(R"({"degrees":90})") == Angle::from_degrees(90.0));
  REQUIRE(read<Angle>(R"({"radians":1.5})") == Angle::from_radians(1.5));

  REQUIRE(read_error<Angle>(R"({})") == glz::error_code::constraint_violated);
  REQUIRE(
    read_error<Angle>(R"({"degrees":1,"radians":1})") == glz::error_code::constraint_violated
  );
  REQUIRE(
    read_error<Angle>(R"({"angle":1,"units":"degrees","degrees":1})")
    == glz::error_code::constraint_violated
  );
  REQUIRE(read_error<Angle>(R"({"angle":1})") == glz::error_code::constraint_violated);
  REQUIRE(read_error<Angle>(R"({"units":"degrees"})") == glz::error_code::constraint_violated);
}

TEST_CASE("Euler angles glaze adapter nests three angles", "[glaze][euler]")
{
  constexpr auto angles = EulerAngles::from_degrees(10.0, 20.0, 30.0);
  auto const json = write(angles);
  REQUIRE(json.starts_with(R"({"roll":{"angle":10)"));
  REQUIRE(read<EulerAngles>(json) == angles);
  REQUIRE(
    strict_read_error<EulerAngles>(R"({"roll":{"angle":1,"units":"degrees"}})")
    != glz::error_code::none
  );
}
