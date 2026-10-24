#pragma once

/**
 * @defgroup core WCommon Core
 * @brief Foundational types and utilities shared by WCommon-based libraries.
 *
 * The core module contains small, dependency-light building blocks: error types, checks, value
 * types, measurement values, adapters, memory wrappers, math helpers, filesystem helpers,
 * threading guards, and functional utilities.
 */

/**
 * @defgroup core_measurements Measurements
 * @ingroup core
 * @brief Units, physical constants, and strongly typed measurement values.
 * @details Every scalar quantity derives from QuantityBase and stores one `double` in its SI unit.
 * See @ref quantities for the list of quantity types, units and literals.
 * @see QuantityBase
 * @see Distance
 * @see Velocity
 */

/**
 * @defgroup core_identifiers Identifiers and Versions
 * @ingroup core
 * @brief UUID and Semantic Versioning value types.
 * @see Uuid
 * @see Version
 */

/**
 * @defgroup core_networking Networking
 * @ingroup core
 * @brief Networking address types and parsing helpers.
 */

/**
 * @defgroup core_logging Logging
 * @ingroup core
 * @brief spdlog and Qt logging setup helpers.
 */

/**
 * @defgroup core_filesystem Filesystem
 * @ingroup core
 * @brief Platform path helpers and filesystem-based locking.
 */

#include <whs31/common/acceleration.h>
#include <whs31/common/adapter.h>
#include <whs31/common/angle.h>
#include <whs31/common/angular_acceleration.h>
#include <whs31/common/angular_velocity.h>
#include <whs31/common/bit.h>
#include <whs31/common/capacitance.h>
#include <whs31/common/char_utils.h>
#include <whs31/common/charge.h>
#include <whs31/common/check.h>
#include <whs31/common/concepts.h>
#include <whs31/common/conductance.h>
#include <whs31/common/constants.h>
#include <whs31/common/current.h>
#include <whs31/common/curve.h>
#include <whs31/common/density.h>
#include <whs31/common/distance.h>
#include <whs31/common/energy.h>
#include <whs31/common/error.h>
#include <whs31/common/euler_angles.h>
#include <whs31/common/fatal.h>
#include <whs31/common/force.h>
#include <whs31/common/frequency.h>
#include <whs31/common/functional.h>
#include <whs31/common/geodesy.h>
#include <whs31/common/graph_resolver.h>
#include <whs31/common/ip.h>
#include <whs31/common/lockfile.h>
#include <whs31/common/log.h>
#include <whs31/common/magnetic_flux_density.h>
#include <whs31/common/mass.h>
#include <whs31/common/math.h>
#include <whs31/common/memo.h>
#include <whs31/common/matrix3x3.h>
#include <whs31/common/memory.h>
#include <whs31/common/moment_of_inertia.h>
#include <whs31/common/noise.h>
#include <whs31/common/paths.h>
#include <whs31/common/pimpl.h>
#include <whs31/common/point2.h>
#include <whs31/common/point3.h>
#include <whs31/common/power.h>
#include <whs31/common/pressure.h>
#include <whs31/common/quantity.h>
#include <whs31/common/quaternion.h>
#include <whs31/common/random.h>
#include <whs31/common/resistance.h>
#include <whs31/common/sockaddr.h>
#include <whs31/common/temperature.h>
#include <whs31/common/threading.h>
#include <whs31/common/torque.h>
#include <whs31/common/uuid.h>
#include <whs31/common/vector2.h>
#include <whs31/common/vector3.h>
#include <whs31/common/velocity.h>
#include <whs31/common/version.h>
#include <whs31/common/version_range.h>
#include <whs31/common/voltage.h>
#include <whs31/common/web_mercator.h>
