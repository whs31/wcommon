#pragma once

#include <string>
#include <utility>

#include <fmt/format.h>

#include <whs31/common/detail/export.h>
#include <whs31/common/detail/namespace.h>

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Reports an unrecoverable error and terminates the process with `EXIT_FAILURE`.
 * @details Shuts the loggers down first (shutdown_log()), so that the messages asynchronous
 * loggers still hold are written before the report rather than lost or printed after it, then
 * prints `fatal: <message>` in bold to stderr and calls `std::exit`. Destructors of automatic
 * objects do not run, scope guards included.
 *
 * Meant for the top level of an application: an invalid command line, or an exception escaping
 * `main()`. Libraries report failures by throwing.
 *
 * @code {.cpp}
 * try {
 *   return run(argc, argv);
 * } catch(std::exception const& error) {
 *   whs31::fatal("{}", error.what());
 * }
 * @endcode
 *
 * @param message Message printed after `fatal:`, as is.
 * @ingroup core_logging
 * @see shutdown_log()
 */
[[noreturn]] WHS31_COMMON_API void fatal(std::string const& message);

/**
 * @brief Formats a message and reports it with fatal(std::string const&).
 * @tparam Args Format argument types.
 * @param fmt Format string.
 * @param args Format arguments.
 * @ingroup core_logging
 */
template <typename... Args>
[[noreturn]] void fatal(fmt::format_string<Args...> fmt, Args&&... args)
{
  fatal(fmt::format(fmt, std::forward<Args>(args)...));
}

WHS31_COMMON_NAMESPACE_END
