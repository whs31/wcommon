#include <radar/mms/common/json/acceleration.h>
#include <radar/mms/common/json/angular_acceleration.h>
#include <radar/mms/common/json/angular_velocity.h>
#include <radar/mms/common/json/capacitance.h>
#include <radar/mms/common/json/charge.h>
#include <radar/mms/common/json/conductance.h>
#include <radar/mms/common/json/current.h>
#include <radar/mms/common/json/density.h>
#include <radar/mms/common/json/energy.h>
#include <radar/mms/common/json/force.h>
#include <radar/mms/common/json/frequency.h>
#include <radar/mms/common/json/magnetic_flux_density.h>
#include <radar/mms/common/json/mass.h>
#include <radar/mms/common/json/moment_of_inertia.h>
#include <radar/mms/common/json/power.h>
#include <radar/mms/common/json/pressure.h>
#include <radar/mms/common/json/resistance.h>
#include <radar/mms/common/json/temperature.h>
#include <radar/mms/common/json/torque.h>
#include <radar/mms/common/json/voltage.h>

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

namespace
{
  template <typename T>
  void check_round_trip(std::string const& key)
  {
    auto const value = T(12.5);
    auto const json = nlohmann::json(value);
    REQUIRE(json == nlohmann::json({{key, 12.5}}));
    REQUIRE(json.get<T>() == value);
  }

  template <typename T>
  void check_rejects_invalid(std::string const& key)
  {
    REQUIRE_THROWS(nlohmann::json({{key, "12.5"}}).get<T>());
    REQUIRE_THROWS(nlohmann::json({{key, nullptr}}).get<T>());
    REQUIRE_THROWS(nlohmann::json({{"value", 12.5}}).get<T>());
  }

  template <typename T>
  void check_schema(std::string const& key)
  {
    check_round_trip<T>(key);
    check_rejects_invalid<T>(key);
  }
}  // namespace

TEST_CASE("Quantity JSON adapters store the SI magnitude under the unit name", "[quantities][json]")
{
  check_schema<Mass>("kilograms");
  check_schema<Force>("newtons");
  check_schema<Torque>("newton_meters");
  check_schema<Pressure>("pascals");
  check_schema<Temperature>("kelvin");
  check_schema<Density>("kilograms_per_cubic_meter");
  check_schema<AngularVelocity>("radians_per_second");
  check_schema<AngularAcceleration>("radians_per_second_squared");
  check_schema<Acceleration>("meters_per_second_squared");
  check_schema<Frequency>("hertz");
  check_schema<MomentOfInertia>("kilogram_square_meters");
  check_schema<Voltage>("volts");
  check_schema<Current>("amperes");
  check_schema<Resistance>("ohms");
  check_schema<Conductance>("siemens");
  check_schema<Power>("watts");
  check_schema<Energy>("joules");
  check_schema<Charge>("coulombs");
  check_schema<Capacitance>("farads");
  check_schema<MagneticFluxDensity>("teslas");
}

TEST_CASE("Quantity JSON adapters work inside containers", "[quantities][json]")
{
  auto const config = nlohmann::json::parse(R"({"pressure": {"pascals": 101325.0}, "cells": [
    {"volts": 3.7}, {"volts": 4.2}
  ]})");

  REQUIRE(config.at("pressure").get<Pressure>() == Pressure::from_hectopascals(1'013.25));
  auto const cells = config.at("cells").get<std::vector<Voltage>>();
  REQUIRE(cells.size() == 2);
  REQUIRE(cells.back() == Voltage::from_volts(4.2));
}
