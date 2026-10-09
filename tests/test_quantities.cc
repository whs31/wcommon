#include <radar/mms/common/acceleration.h>
#include <radar/mms/common/angle.h>
#include <radar/mms/common/angular_acceleration.h>
#include <radar/mms/common/angular_velocity.h>
#include <radar/mms/common/capacitance.h>
#include <radar/mms/common/charge.h>
#include <radar/mms/common/concepts.h>
#include <radar/mms/common/conductance.h>
#include <radar/mms/common/current.h>
#include <radar/mms/common/density.h>
#include <radar/mms/common/distance.h>
#include <radar/mms/common/energy.h>
#include <radar/mms/common/force.h>
#include <radar/mms/common/frequency.h>
#include <radar/mms/common/magnetic_flux_density.h>
#include <radar/mms/common/mass.h>
#include <radar/mms/common/math.h>
#include <radar/mms/common/moment_of_inertia.h>
#include <radar/mms/common/power.h>
#include <radar/mms/common/pressure.h>
#include <radar/mms/common/resistance.h>
#include <radar/mms/common/temperature.h>
#include <radar/mms/common/torque.h>
#include <radar/mms/common/velocity.h>
#include <radar/mms/common/voltage.h>

#include <bit>
#include <chrono>
#include <limits>
#include <numbers>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>

#include <fmt/format.h>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;
using namespace std::chrono_literals;

namespace
{
  template <typename T>
  concept SelfMultipliable = requires(T value) { value * value; };

  template <typename T, typename U>
  concept Addable = requires(T lhs, U rhs) { lhs + rhs; };

  template <typename T, typename U>
  concept EqualityComparable = requires(T lhs, U rhs) { lhs == rhs; };

  template <typename T>
  concept Negatable = requires(T value) { -value; };

  constexpr auto tolerance = 1e-9;

  constexpr bool near(double const actual, double const expected, double const epsilon = tolerance)
  {
    return approx_eq(actual, expected, epsilon);
  }
}  // namespace

TEMPLATE_TEST_CASE(
  "Every quantity is one SI double with the common operations",
  "[quantities]",
  Angle,
  Distance,
  Velocity,
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

  STATIC_CHECK(Quantity<T>);
  STATIC_CHECK(DerivedQuantity<T>);
  STATIC_CHECK(std::is_trivially_copyable_v<T>);
  STATIC_CHECK(std::is_standard_layout_v<T>);
  STATIC_CHECK(sizeof(T) == sizeof(double));
  STATIC_CHECK(std::bit_cast<double>(T(2.5)) == 2.5);
  STATIC_CHECK(QuantityTraits<T>::from_si(2.5) == T(2.5));
  STATIC_CHECK(QuantityTraits<T>::si_symbol == T::si_symbol);

  STATIC_CHECK(T::zero() == T());
  STATIC_CHECK(T(2.0) + T(3.0) == T(5.0));
  STATIC_CHECK(T(2.0) - T(3.0) == T(-1.0));
  STATIC_CHECK(T(2.0) * 3.0 == T(6.0));
  STATIC_CHECK(3.0 * T(2.0) == T(6.0));
  STATIC_CHECK(T(6.0) / 3.0 == T(2.0));
  STATIC_CHECK(T(6.0) / T(3.0) == 2.0);
  STATIC_CHECK(T(2.0) < T(3.0));
  STATIC_CHECK(static_cast<double>(T(-4.0).abs()) == 4.0);
  STATIC_CHECK(T(1.0).is_finite());
  STATIC_CHECK_FALSE(T(std::numeric_limits<double>::infinity()).is_finite());
  STATIC_CHECK_FALSE(T(std::numeric_limits<double>::quiet_NaN()).is_finite());
  STATIC_CHECK_FALSE(T(std::numeric_limits<double>::quiet_NaN()) == T(0.0));

  STATIC_CHECK_FALSE(SelfMultipliable<T>);
  STATIC_CHECK_FALSE(std::is_convertible_v<double, T>);
  STATIC_CHECK_FALSE(std::is_convertible_v<T, double>);

  auto value = T(1.0);
  value += T(2.0);
  value -= T(0.5);
  value *= 4.0;
  value /= 2.0;
  REQUIRE(value == T(5.0));

  auto stream = std::ostringstream();
  stream << value;
  REQUIRE(stream.str() == value.to_string());
  REQUIRE(fmt::format("{}", value) == value.to_string());

  auto values = std::unordered_set<T> {T(3.0)};
  REQUIRE(values.contains(T(3.0)));
}

TEST_CASE("Quantities of different types never mix", "[quantities]")
{
  STATIC_CHECK_FALSE(Addable<Mass, Force>);
  STATIC_CHECK_FALSE(Addable<Voltage, Current>);
  STATIC_CHECK_FALSE(Addable<Mass, double>);
  STATIC_CHECK_FALSE(EqualityComparable<Power, Energy>);
  STATIC_CHECK_FALSE(EqualityComparable<Torque, double>);

  // The legacy quantities keep their raw-double operators in their SI unit.
  STATIC_CHECK(Distance::from_meters(2.0) + 1.0 == Distance::from_meters(3.0));
  STATIC_CHECK(1.0 - Velocity::from_mps(3.0) == Velocity::from_mps(-2.0));
  STATIC_CHECK(Angle::from_radians(1.0) + 1.0 == Angle::from_radians(2.0));
}

TEST_CASE("Quantities format the magnitude with the SI symbol", "[quantities]")
{
  REQUIRE(fmt::format("{}", 2.5_kg) == "2.5 kg");
  REQUIRE(fmt::format("{}", 1_kn) == "1000 N");
  REQUIRE(fmt::format("{}", 3_nm) == "3 N*m");
  REQUIRE(fmt::format("{}", 1013.25_hpa) == "101325 Pa");
  REQUIRE(fmt::format("{}", 300_k) == "300 K");
  REQUIRE(fmt::format("{}", 1.225_kg_per_m3) == "1.225 kg/m^3");
  REQUIRE(fmt::format("{}", 2_rad_per_s) == "2 rad/s");
  REQUIRE(fmt::format("{}", 2_rad_per_s2) == "2 rad/s^2");
  REQUIRE(fmt::format("{}", 1_gn) == "9.80665 m/s^2");
  REQUIRE(fmt::format("{}", 400_hz) == "400 Hz");
  REQUIRE(fmt::format("{}", 0.5_kg_m2) == "0.5 kg*m^2");
  REQUIRE(fmt::format("{}", 12_v) == "12 V");
  REQUIRE(fmt::format("{}", 3_a) == "3 A");
  REQUIRE(fmt::format("{}", 2_ohm) == "2 ohm");
  REQUIRE(fmt::format("{}", 0.5_siemens) == "0.5 S");
  REQUIRE(fmt::format("{}", 1_kw) == "1000 W");
  REQUIRE(fmt::format("{}", 1_wh) == "3600 J");
  REQUIRE(fmt::format("{}", 1_ah) == "3600 C");
  REQUIRE(fmt::format("{}", 470_uf) == "0.00047 F");
  REQUIRE(fmt::format("{}", 0.5_gauss) == "5e-05 T");
  REQUIRE(fmt::format("{:>8}", 12_v) == "    12 V");

  // Angle and Distance keep their own display units.
  REQUIRE(fmt::format("{}", 90_deg) == "90 deg");
  REQUIRE(fmt::format("{}", 1'500_m) == "1.5 km");
}

TEST_CASE("Mechanical quantities convert between units", "[quantities]")
{
  STATIC_CHECK((2.5_kg + 300_g).as_kilograms() == 2.8);
  STATIC_CHECK(near(Mass::from_kilograms(1.5).as_grams(), 1'500.0));

  STATIC_CHECK(Force::from_kilonewtons(1.2).as_newtons() == 1'200.0);
  STATIC_CHECK(near((1_kgf).as_newtons(), 9.806'65));
  STATIC_CHECK(near(Force::from_newtons(9.806'65).as_kilograms_force(), 1.0));
  STATIC_CHECK(near((2_n).as_kilonewtons(), 0.002));

  STATIC_CHECK((1.5_nm).as_newton_meters() == 1.5);

  STATIC_CHECK((1013.25_hpa).as_pascals() == 101'325.0);
  STATIC_CHECK(near((101.325_kpa).as_hectopascals(), 1'013.25));
  STATIC_CHECK(near((1013.25_mbar).as_kilopascals(), 101.325));
  STATIC_CHECK(near((1013.25_hpa).as_millibars(), 1'013.25));
  STATIC_CHECK(near((29.92_inhg).as_hectopascals(), 1'013.207'589, 1e-6));
  STATIC_CHECK(near(Pressure::from_pascals(101'325.0).as_inches_of_mercury(), 29.921'252, 1e-6));

  STATIC_CHECK((1.225_kg_per_m3).as_kilograms_per_cubic_meter() == 1.225);
  STATIC_CHECK((0.35_kg_m2).as_kilogram_square_meters() == 0.35);
}

TEST_CASE("Temperature is absolute and converts to Celsius and Fahrenheit", "[quantities]")
{
  STATIC_CHECK((0_degc).as_kelvin() == 273.15);
  STATIC_CHECK(near((15_degc).as_kelvin(), 288.15));
  STATIC_CHECK(near(Temperature::from_celsius(-40.0).as_fahrenheit(), -40.0));
  STATIC_CHECK(near((212_degf).as_celsius(), 100.0));
  STATIC_CHECK(near((32_degf).as_kelvin(), 273.15));
  STATIC_CHECK(near((300_k).as_celsius(), 26.85));

  // A difference is a Temperature in kelvin; only its kelvin value is meaningful.
  STATIC_CHECK(near((25_degc - 15_degc).as_kelvin(), 10.0));

  // `-5_degc` would be `-(5_degc)`, i.e. -278.15 K, so negation does not compile.
  STATIC_CHECK_FALSE(Negatable<Temperature>);
  STATIC_CHECK(Negatable<Mass>);
}

TEST_CASE("Rotational quantities convert between units", "[quantities]")
{
  STATIC_CHECK(near((180_deg_per_s).as_radians_per_second(), std::numbers::pi_v<double>));
  STATIC_CHECK(near((60_rpm).as_radians_per_second(), 2.0 * std::numbers::pi_v<double>));
  STATIC_CHECK(near((60_rpm).as_degrees_per_second(), 360.0));
  STATIC_CHECK(
    near(AngularVelocity::from_radians_per_second(std::numbers::pi_v<double>).as_rpm(), 30.0)
  );

  STATIC_CHECK(
    near((90_deg_per_s2).as_radians_per_second_squared(), std::numbers::pi_v<double> / 2.0)
  );
  STATIC_CHECK(
    near((1_rad_per_s2).as_degrees_per_second_squared(), 180.0 / std::numbers::pi_v<double>)
  );
}

TEST_CASE("Kinematic quantities relate through std::chrono durations", "[quantities]")
{
  STATIC_CHECK(100_m / 4s == Velocity::from_mps(25.0));
  STATIC_CHECK(1_km / 500ms == Velocity::from_mps(2'000.0));
  STATIC_CHECK(Velocity::from_mps(25.0) * 2s == Distance::from_meters(50.0));
  STATIC_CHECK(2s * Velocity::from_mps(25.0) == Distance::from_meters(50.0));
  STATIC_CHECK(100_m / Velocity::from_mps(25.0) == std::chrono::duration<double>(4.0));
  STATIC_CHECK(
    std::chrono::duration_cast<std::chrono::milliseconds>(10_m / Velocity::from_mps(4.0)) == 2'500ms
  );

  STATIC_CHECK(Velocity::from_mps(10.0) / 2s == 5_mps2);
  STATIC_CHECK(near((1_gn * 2s).as_mps(), 19.613'3));
  STATIC_CHECK(250ms * 4_mps2 == Velocity::from_mps(1.0));

  STATIC_CHECK(near((90_deg / 2s).as_degrees_per_second(), 45.0));
  STATIC_CHECK(near((90_deg_per_s * 2s).degrees(), 180.0));
  STATIC_CHECK(near((500ms * 2_rad_per_s).radians(), 1.0));

  STATIC_CHECK(10_rad_per_s / 2s == 5_rad_per_s2);
  STATIC_CHECK(10_rad_per_s2 * 3s == 30_rad_per_s);
  STATIC_CHECK(3s * 10_rad_per_s2 == 30_rad_per_s);
}

TEST_CASE("Acceleration converts to standard gravity", "[quantities]")
{
  STATIC_CHECK((1_gn).as_mps2() == STANDARD_GRAVITY);
  STATIC_CHECK(near(Acceleration::from_mps2(19.613'3).as_standard_gravity(), 2.0));
}

TEST_CASE("Frequency converts to and from a period", "[quantities]")
{
  STATIC_CHECK((2_khz).as_hertz() == 2'000.0);
  STATIC_CHECK(near((500_hz).as_kilohertz(), 0.5));
  STATIC_CHECK((400_hz).period() == std::chrono::duration<double>(0.002'5));
  STATIC_CHECK(Frequency::from_period(16ms) == 62.5_hz);
  STATIC_CHECK(Frequency::from_period(std::chrono::duration<double>(0.5)) == 2_hz);
  REQUIRE((0_hz).period().count() == std::numeric_limits<double>::infinity());
}

TEST_CASE("Electrical quantities convert between units", "[quantities]")
{
  STATIC_CHECK(near((3.7_v * 6.0).as_volts(), 22.2));
  STATIC_CHECK(near((3'700_mv).as_volts(), 3.7));
  STATIC_CHECK(near((3.7_v).as_millivolts(), 3'700.0));

  STATIC_CHECK(near((12_a + 350_ma).as_amperes(), 12.35));
  STATIC_CHECK(near((0.25_a).as_milliamperes(), 250.0));

  STATIC_CHECK(near((45_mohm).as_ohms(), 0.045));
  STATIC_CHECK(near((2_ohm).as_milliohms(), 2'000.0));

  STATIC_CHECK((0.25_siemens).as_siemens() == 0.25);

  STATIC_CHECK((1.8_kw + 150_w).as_watts() == 1'950.0);
  STATIC_CHECK(near((500_w).as_kilowatts(), 0.5));

  STATIC_CHECK((222_wh).as_joules() == 799'200.0);
  STATIC_CHECK(near((1_kwh).as_watt_hours(), 1'000.0));
  STATIC_CHECK(near(Energy::from_joules(7'200.0).as_kilowatt_hours(), 0.002));

  STATIC_CHECK(near((5'000_mah).as_ampere_hours(), 5.0));
  STATIC_CHECK(near((5'000_mah).as_coulombs(), 18'000.0));
  STATIC_CHECK(near((2_ah).as_milliampere_hours(), 2'000.0));

  STATIC_CHECK(near((470_uf).as_farads(), 0.000'47, 1e-15));
  STATIC_CHECK(near((1_f).as_microfarads(), 1'000'000.0));
}

TEST_CASE("Magnetic flux density converts between tesla and gauss", "[quantities]")
{
  STATIC_CHECK(near((0.5_gauss).as_microteslas(), 50.0));
  STATIC_CHECK(near((50_ut).as_gauss(), 0.5));
  STATIC_CHECK(near((50'000_nt).as_microteslas(), 50.0));
  STATIC_CHECK(near((1_ut).as_nanoteslas(), 1'000.0));
  STATIC_CHECK(near((1_gauss).as_teslas(), 0.000'1, 1e-15));
}

TEST_CASE("Distance has short literals, ratios, abs and finite checks", "[quantities]")
{
  STATIC_CHECK(1_m == Distance::from_meters(1.0));
  STATIC_CHECK(1.5_km == Distance::from_kilometers(1.5));
  STATIC_CHECK(near((25_cm).as_m(), 0.25));
  STATIC_CHECK(near((5_mm).as_m(), 0.005));
  STATIC_CHECK(2_meters == 2_m);
  STATIC_CHECK(3_kilometers == 3_km);

  STATIC_CHECK(50_m / 10_m == 5.0);
  STATIC_CHECK(Distance::from_meters(-3.0).abs() == 3_m);
  STATIC_CHECK(abs(Distance::from_meters(-3.0)) == 3_m);
  STATIC_CHECK(Velocity::from_mps(-2.0).abs() == 2_mps);
  STATIC_CHECK(Distance::from_meters(1.0).is_finite());
  STATIC_CHECK_FALSE(Velocity::from_mps(std::numeric_limits<double>::infinity()).is_finite());
}

TEST_CASE("Math helpers accept quantities", "[quantities][math]")
{
  STATIC_CHECK(clamp(15_v, 0_v, 12_v) == 12_v);
  STATIC_CHECK(clamp(-1_a, 0_a, 2_a) == 0_a);
  STATIC_CHECK(clamp(1_a, 2_a, 0_a) == 1_a);
  STATIC_CHECK(lerp(10_m, 20_m, 0.5) == 15_m);
  STATIC_CHECK(approx_eq(lerp(0_degc, 100_degc, 0.25), 25_degc, 1e-9_k));
  STATIC_CHECK(
    near(lerp(Angle::from_degrees(350.0), Angle::from_degrees(10.0), 0.5).degrees(), 180.0)
  );

  STATIC_CHECK(approx_eq(0.1_kg + 0.2_kg, 0.3_kg));
  STATIC_CHECK_FALSE(approx_eq(1_kg, 1.001_kg));
  STATIC_CHECK(approx_eq(1_kg, 1.001_kg, 2_g));
  STATIC_CHECK(is_null(Mass::from_kilograms(1e-17)));
  STATIC_CHECK_FALSE(is_null(1_g));
  STATIC_CHECK(is_null(1_g, 2_g));

  STATIC_CHECK(positive(1_hz));
  STATIC_CHECK(nonnegative(0_w));
  STATIC_CHECK(in_closed_range(15_degc, Temperature::from_celsius(-40.0), 400_k));
  STATIC_CHECK_FALSE(is_finite(Pressure::from_pascals(std::numeric_limits<double>::infinity())));

  STATIC_CHECK(std::min(2_v, 3_v) == 2_v);
  STATIC_CHECK(std::max(2_v, 3_v) == 3_v);
}
