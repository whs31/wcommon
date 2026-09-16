#pragma once

// Qt's moc does not support the C++23 #elifdef directive yet.
// NOLINTBEGIN(readability-use-concise-preprocessor-directives)
#ifdef __DOXYGEN__
#  define RADAR_COMMON_NAMESPACE_BEGIN namespace radar::mms { namespace common {
#  define RADAR_COMMON_NAMESPACE_END } }
#elif defined(__cplusplus)
#  define RADAR_COMMON_NAMESPACE_BEGIN namespace radar::mms { inline namespace common {
#  define RADAR_COMMON_NAMESPACE_END } }
#else
#  define RADAR_COMMON_NAMESPACE_BEGIN
#  define RADAR_COMMON_NAMESPACE_END
#endif
// NOLINTEND(readability-use-concise-preprocessor-directives)
#define RADAR_COMMON_FORWARD_DECLARE(TYPE) /* NOLINT(cppcoreguidelines-macro-usage) */ \
  RADAR_COMMON_NAMESPACE_BEGIN TYPE; RADAR_COMMON_NAMESPACE_END

/**
 * @defgroup macros Macros
 * @ingroup core
 * @brief Public preprocessor helpers and configuration flags.
 */

#ifdef __DOXYGEN__
/**
 * @ingroup macros
 * @brief Flag defined if debug mode is **enabled**.
 * @details Enables some additional checks, the majority of contract functionality and some
 * assertions.
 * @see RADAR_NO_DEBUG
 */
#  define RADAR_DEBUG

/**
 * @ingroup macros
 * @brief Flag defined if debug mode is **disabled**.
 * @details Disables some additional checks, the majority of contract functionality and some
 * assertions.
 * @see RADAR_DEBUG
 */
#  define RADAR_NO_DEBUG
#endif
