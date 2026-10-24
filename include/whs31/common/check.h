#pragma once

#include <cstdlib>
#include <cassert>

#include <whs31/common/config.h>
#include <whs31/common/error.h>

// NOLINTBEGIN(*-reserved-identifier, *-identifier-naming, *-macro-usage)

/**
 * @ingroup macros
 * @brief Performs a debug-only runtime check.
 * @details In debug builds, throws `EXCEPTION_TYPE(__VA_ARGS__)` when `x`
 * evaluates to `false`. In release builds, the expression and constructor
 * arguments are not evaluated.
 * @param x Boolean expression to check.
 * @param EXCEPTION_TYPE Exception type to construct on failure.
 * @param ... Arguments forwarded to the exception constructor.
 * @see WHS31_HARD_CHECK
 */
#ifdef WHS31_DEBUG
#  define WHS31_SOFT_CHECK(x, EXCEPTION_TYPE, ...)                                              \
    do {                                                                                        \
      if(not (x))                                                                               \
        throw EXCEPTION_TYPE(__VA_ARGS__);                                                      \
    } while(false)
#else
#  define WHS31_SOFT_CHECK(x, EXCEPTION_TYPE, ...)                                              \
    do {                                                                                        \
    } while(false)
#endif

/**
 * @ingroup macros
 * @brief Performs an unconditional runtime check.
 * @details Throws `EXCEPTION_TYPE(__VA_ARGS__)` when `x` evaluates to `false`
 * in every build configuration.
 * @param x Boolean expression to check.
 * @param EXCEPTION_TYPE Exception type to construct on failure.
 * @param ... Arguments forwarded to the exception constructor.
 * @see WHS31_SOFT_CHECK
 */
#define WHS31_HARD_CHECK(x, EXCEPTION_TYPE, ...)                                                \
  do {                                                                                          \
    if(not (x))                                                                                 \
      throw EXCEPTION_TYPE(__VA_ARGS__);                                                        \
  } while(false)

/**
 * @ingroup macros
 * @brief Throws `whs31::common::NotImplementedError`.
 * @details Uses the exception's default source-location argument, so the
 * reported location is the expansion site.
 */
#define WHS31_NOT_IMPLEMENTED throw whs31::common::NotImplementedError();

// NOLINTEND(*-reserved-identifier, *-identifier-naming, *-macro-usage)
