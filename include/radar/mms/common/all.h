#pragma once

/**
 * @defgroup core Radar Common Core
 * @brief Foundational types and utilities shared by Radar MMS libraries.
 *
 * The core module contains small, dependency-light building blocks: error types, checks, value
 * types, measurement values, adapters, memory wrappers, math helpers, filesystem helpers,
 * threading guards, and functional utilities.
 */

/**
 * @defgroup core_measurements Measurements
 * @ingroup core
 * @brief Units, physical constants, and strongly typed measurement values.
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

#include <radar/mms/common/adapter.h>
#include <radar/mms/common/angle.h>
#include <radar/mms/common/char_utils.h>
#include <radar/mms/common/check.h>
#include <radar/mms/common/concepts.h>
#include <radar/mms/common/constants.h>
#include <radar/mms/common/curve.h>
#include <radar/mms/common/distance.h>
#include <radar/mms/common/error.h>
#include <radar/mms/common/euler_angles.h>
#include <radar/mms/common/functional.h>
#include <radar/mms/common/graph_resolver.h>
#include <radar/mms/common/ip.h>
#include <radar/mms/common/lockfile.h>
#include <radar/mms/common/log.h>
#include <radar/mms/common/math.h>
#include <radar/mms/common/matrix3x3.h>
#include <radar/mms/common/memory.h>
#include <radar/mms/common/paths.h>
#include <radar/mms/common/pimpl.h>
#include <radar/mms/common/point2.h>
#include <radar/mms/common/point3.h>
#include <radar/mms/common/quaternion.h>
#include <radar/mms/common/sockaddr.h>
#include <radar/mms/common/threading.h>
#include <radar/mms/common/uuid.h>
#include <radar/mms/common/vector2.h>
#include <radar/mms/common/vector3.h>
#include <radar/mms/common/version.h>
#include <radar/mms/common/version_range.h>
#include <radar/mms/common/velocity.h>
