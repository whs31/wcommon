#pragma once

#include <numbers>

#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN
// NOLINTBEGIN(readability-identifier-naming)

/**
 * @brief Number of seconds in a minute.
 * @ingroup core_measurements
 */
inline auto constexpr SECONDS_MINUTES_FACTOR = 60.0;

/**
 * @brief Number of minutes in an hour.
 * @ingroup core_measurements
 */
inline auto constexpr MINUTES_HOURS_FACTOR = 60.0;

/**
 * @brief Number of seconds in an hour.
 * @ingroup core_measurements
 */
inline auto constexpr SECONDS_HOURS_FACTOR = SECONDS_MINUTES_FACTOR * MINUTES_HOURS_FACTOR;

/**
 * @brief Number of nanometers in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_NANOMETER_FACTOR = 1'000'000'000.0;

/**
 * @brief Number of micrometers in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_MICROMETER_FACTOR = 1'000'000.0;

/**
 * @brief Number of millimeters in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_MILLIMETER_FACTOR = 1'000.0;

/**
 * @brief Number of centimeters in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_CENTIMETER_FACTOR = 100.0;

/**
 * @brief Number of decimeters in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_DECIMETER_FACTOR = 10.0;

/**
 * @brief Number of hectometers in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_HECTOMETER_FACTOR = 0.01;

/**
 * @brief Number of kilometers in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_KILOMETER_FACTOR = 0.001;

/**
 * @brief Number of inches in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_INCH_FACTOR = 10'000.0 / 254.0;

/**
 * @brief Number of feet in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_FEET_FACTOR = 10'000.0 / (254.0 * 12.0);

/**
 * @brief Number of yards in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_YARD_FACTOR = 10'000.0 / (254.0 * 12.0 * 3.0);

/**
 * @brief Number of furlongs in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_FURLONG_FACTOR = 10'000.0 / (254.0 * 12.0 * 3.0 * 220.0);

/**
 * @brief Number of miles in a meter.
 * @ingroup core_measurements
 */
inline auto constexpr METER_MILE_FACTOR = 10'000.0 / (254.0 * 12.0 * 3.0 * 1'760.0);

/**
 * @brief Standard acceleration of gravity g₀ in meters per second squared.
 * @details Also the number of newtons in a kilogram-force.
 * @ingroup core_measurements
 */
inline auto constexpr STANDARD_GRAVITY = 9.806'65;

/**
 * @brief Temperature of 0 °C in kelvin.
 * @ingroup core_measurements
 */
inline auto constexpr CELSIUS_ZERO_KELVIN = 273.15;

/**
 * @brief Number of pascals in a conventional inch of mercury.
 * @ingroup core_measurements
 */
inline auto constexpr INHG_PASCAL_FACTOR = 3'386.389;

/**
 * @brief Number of gauss in a tesla.
 * @ingroup core_measurements
 */
inline auto constexpr TESLA_GAUSS_FACTOR = 10'000.0;

/**
 * @brief Speed of light in meters per second.
 * @ingroup core_measurements
 */
inline auto constexpr SPEED_OF_LIGHT = 299'792'458.0;

/**
 * @brief Speed of light squared.
 * @ingroup core_measurements
 */
inline auto constexpr SPEED_OF_LIGHT_SQUARED = SPEED_OF_LIGHT * SPEED_OF_LIGHT;

/**
 * @brief Mean Earth radius in meters.
 * @ingroup core_measurements
 */
inline auto constexpr EARTH_RADIUS = 6'371'007.2;

/**
 * @brief Mean Earth radius in kilometers.
 * @ingroup core_measurements
 */
inline auto constexpr EARTH_RADIUS_KM = EARTH_RADIUS / 1'000.0;

/**
 * @brief Mean Earth circumference in meters.
 * @ingroup core_measurements
 */
inline auto constexpr EARTH_CIRCUMFERENCE = EARTH_RADIUS * 2.0 * std::numbers::pi_v<double>;

/**
 * @brief Mean Earth circumference in kilometers.
 * @ingroup core_measurements
 */
inline auto constexpr EARTH_CIRCUMFERENCE_KM = EARTH_CIRCUMFERENCE / 1'000.0;

// NOLINTEND(readability-identifier-naming)
RADAR_COMMON_NAMESPACE_END
