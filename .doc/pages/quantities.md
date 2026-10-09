# Physical quantities {#quantities}

RCommon has a named type for each common scalar physical quantity, such as `Distance`, `Mass` or
`Voltage`. A quantity stores one `double` in its SI unit and cannot be mixed with a quantity of
another type, so a torque cannot be passed where a force is expected, and a magnitude in grams
cannot be read back as kilograms.

## Common operations

Every quantity derives from `QuantityBase` and has the same operations:

- `==` and `<=>` between equal types, with the floating-point semantics of the magnitudes;
- `+` and `-` between equal types, and `*` and `/` by a dimensionless `double`;
- `a / b` of two quantities of one type, which yields their dimensionless `double` ratio;
- `is_finite()`, `abs()`, `zero()` and an `explicit operator double()` to the SI magnitude;
- `to_string()`, `std::ostream` output, `fmt` formatting and `std::hash`;
- a Qt gadget with one property per unit plus `isFinite` and `string`, when built
  `with_qt=True`;
- a JSON adapter in `radar/mms/common/json/<quantity>.h` and a glaze adapter in
  `radar/mms/common/glaze/<quantity>.h`, which both store the SI magnitude under the unit name,
  e.g. `{"kilograms": 2.5}`.

Each quantity is standard layout and trivially copyable, and its object representation is exactly
one `double` in the SI unit. Reflection and recording can treat it as an `F64` in that unit.

The `Quantity` concept matches every quantity. The range checks (`is_finite`, `positive`,
`nonnegative`, `in_closed_range`) and the math helpers `clamp`, `lerp`, `approx_eq` and `is_null`
accept quantities directly. `std::min` and `std::max` work as they are.

## Literals

User-defined literals live in the inline namespace `radar::mms::literals`. Import them alone with
`using namespace radar::mms::literals;`, or together with the rest of the API with
`using namespace radar::mms;`.

```cpp
using namespace radar::mms::literals;

auto const pack = 3.7_v * 6.0;          // 22.2 V
auto const qnh = 1013.25_hpa;           // 101325 Pa
auto const ratio = 50_m / 10_m;         // 5.0
auto const ok = in_closed_range(pack, 18_v, 25.2_v);
```

## Quantity types

| Type | SI unit | Factories and accessors | Literals |
|------|---------|-------------------------|----------|
| `Angle` | rad | `radians`, `degrees` | `_rad`, `_deg` |
| `Distance` | m | `from_meters`/`as_m`, `from_kilometers`/`as_km`, other metric and imperial units | `_m`, `_km`, `_cm`, `_mm`, `_meters`, `_kilometers` |
| `Velocity` | m/s | `mps`, `kmph`, `mph` | `_mps`, `_kmph`, `_mph` |
| `Acceleration` | m/s² | `mps2`, `standard_gravity` | `_mps2`, `_gn` |
| `Mass` | kg | `kilograms`, `grams` | `_kg`, `_g` |
| `Force` | N | `newtons`, `kilonewtons`, `kilograms_force` | `_n`, `_kn`, `_kgf` |
| `Torque` | N·m | `newton_meters` | `_nm` |
| `Pressure` | Pa | `pascals`, `hectopascals`, `kilopascals`, `millibars`, `inches_of_mercury` | `_pa`, `_hpa`, `_kpa`, `_mbar`, `_inhg` |
| `Temperature` | K | `kelvin`, `celsius`, `fahrenheit` | `_k`, `_degc`, `_degf` |
| `Density` | kg/m³ | `kilograms_per_cubic_meter` | `_kg_per_m3` |
| `AngularVelocity` | rad/s | `radians_per_second`, `degrees_per_second`, `rpm` | `_rad_per_s`, `_deg_per_s`, `_rpm` |
| `AngularAcceleration` | rad/s² | `radians_per_second_squared`, `degrees_per_second_squared` | `_rad_per_s2`, `_deg_per_s2` |
| `Frequency` | Hz | `hertz`, `kilohertz`, `period` | `_hz`, `_khz` |
| `MomentOfInertia` | kg·m² | `kilogram_square_meters` | `_kg_m2` |
| `Voltage` | V | `volts`, `millivolts` | `_v`, `_mv` |
| `Current` | A | `amperes`, `milliamperes` | `_a`, `_ma` |
| `Resistance` | Ω | `ohms`, `milliohms` | `_ohm`, `_mohm` (milliohm) |
| `Conductance` | S | `siemens` | `_siemens` |
| `Power` | W | `watts`, `kilowatts` | `_w`, `_kw` |
| `Energy` | J | `joules`, `watt_hours`, `kilowatt_hours` | `_j`, `_wh`, `_kwh` |
| `Charge` | C | `coulombs`, `ampere_hours`, `milliampere_hours` | `_ah`, `_mah` |
| `Capacitance` | F | `farads`, `microfarads` | `_f`, `_uf` |
| `MagneticFluxDensity` | T | `teslas`, `gauss`, `microteslas`, `nanoteslas` | `_gauss`, `_ut`, `_nt` |

A unit `u` has the factory `from_u()` and the accessor `as_u()`, e.g. `Pressure::from_hectopascals`
and `Pressure::as_hectopascals`. The older types keep their established names: `Angle` has the
`radians()` and `degrees()` accessors, and `Distance` has short accessors such as `as_m()`.

Some literal names differ from the plain unit symbol:

- **`_gn`** is standard gravity (g₀ = 9.80665 m/s²), because `_g` is the gram.
- **`_siemens`** is the conductance literal, because `_s` reads as seconds next to the
  `std::chrono` literals.
- **`_mohm`** is a milliohm, not a megaohm.

## Relations through time

Kinematic quantities relate through `std::chrono` durations of any representation:

| Expression | Result |
|------------|--------|
| `Distance / duration` | `Velocity` |
| `Velocity * duration`, `duration * Velocity` | `Distance` |
| `Distance / Velocity` | `std::chrono::duration<double>` |
| `Velocity / duration` | `Acceleration` |
| `Acceleration * duration`, `duration * Acceleration` | `Velocity` |
| `Angle / duration` | `AngularVelocity` |
| `AngularVelocity * duration`, `duration * AngularVelocity` | `Angle` |
| `AngularVelocity / duration` | `AngularAcceleration` |
| `AngularAcceleration * duration`, `duration * AngularAcceleration` | `AngularVelocity` |

`Frequency::period()` returns the period as `std::chrono::duration<double>`, and
`Frequency::from_period()` accepts any duration.

```cpp
using namespace std::chrono_literals;
using namespace radar::mms::literals;

auto const speed = 100_m / 4s;               // 25 m/s
auto const turned = 90_deg_per_s * 500ms;    // 45 deg
auto const step = (400_hz).period();         // 2.5 ms
```

Other products of quantities, e.g. `Voltage * Current`, are not defined. Coefficients such as a
torque constant in N·m/A stay `double`, named with a unit suffix.

## Temperature

`Temperature` is an absolute temperature in kelvin. The difference of two temperatures is also a
`Temperature`, whose `as_kelvin()` is the difference in kelvin or degrees Celsius. Its
`as_celsius()` and `as_fahrenheit()` have no meaning, since those scales are offset from absolute
zero.

Negation is deleted. Because C++ parses `-5_degc` as `-(5_degc)`, which would be −278.15 K, a
negative Celsius literal does not compile. Write `Temperature::from_celsius(-5.0)` instead.

## Adding a quantity

A project can add its own quantity by deriving from `QuantityBase`. The derived type then
satisfies `Quantity` and gets every common operation. See `QuantityBase` for an example.
