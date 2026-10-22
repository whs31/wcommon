#pragma once

#include <exception>
#include <source_location>
#include <string>
#include <system_error>
#include <utility>

#include <fmt/format.h>

#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @defgroup core_errors Errors
 * @ingroup core
 * @brief Exception types used by Radar core APIs.
 * @see Error
 */

/**
 * @brief Base class for project-wide exceptions.
 * @ingroup core_errors
 * @see SystemError
 */
class Error : public std::exception
{
 protected:
  std::string m_message;  // NOLINT(*-non-private-member-variables-in-classes)

 public:
  template <typename... Args>
  explicit Error(fmt::format_string<Args...> fmt, Args&&... args)
    : m_message(fmt::format(fmt, std::forward<Args>(args)...))
  {}

  [[nodiscard]] char const* what() const noexcept override { return this->m_message.c_str(); }
};

/**
 * @brief Error wrapper for std::system_error.
 * @ingroup core_errors
 */
class SystemError : public Error
{
 public:
  explicit SystemError(std::system_error const& e)
    : Error("system error {}: {}", e.code().value(), e.what())
  {}
};

/**
 * @brief Error thrown when a caller-supplied value violates an API precondition.
 * @details For example, a lookup table with unordered arguments or a random bound of zero.
 * @ingroup core_errors
 * @see NumericError
 */
class InvalidArgumentError final : public Error
{
 public:
  using Error::Error;
};

/**
 * @brief Error thrown when a computation has no finite, well-defined result.
 * @details For example, inverting a singular matrix or normalizing a zero quaternion.
 * @ingroup core_errors
 * @see InvalidArgumentError
 */
class NumericError final : public Error
{
 public:
  using Error::Error;
};

/**
 * @brief Error thrown for code paths that are intentionally not implemented yet.
 * @ingroup core_errors
 * @see RADAR_NOT_IMPLEMENTED
 */
class NotImplementedError final : public Error
{
 public:
  explicit NotImplementedError(
    std::source_location const& source_loc = std::source_location::current()
  )
    : Error(
        "not implemented (file {}, at {}:{} [{}])",
        source_loc.file_name(),
        source_loc.line(),
        source_loc.column(),
        source_loc.function_name()
      )
  {}
};

/**
 * @brief Returns the message of a captured exception.
 * @details Useful where an exception crosses a thread or a callback as a `std::exception_ptr` and
 * only its message is reported, e.g. in a log line or a notification.
 *
 * @code {.cpp}
 * auto const error = std::make_exception_ptr(radar::mms::NumericError("singular matrix"));
 * auto const message = radar::mms::describe(error);  // "singular matrix"
 * @endcode
 *
 * @param error Captured exception, or null.
 * @return `what()` of a `std::exception`, `unknown exception` for anything else thrown, and an
 * empty string for null.
 * @ingroup core_errors
 */
[[nodiscard]] inline std::string describe(std::exception_ptr const& error)
{
  if(not error)
    return {};
  try {
    std::rethrow_exception(error);
  } catch(std::exception const& exception) {
    return exception.what();
  } catch(...) {
    return "unknown exception";
  }
}

RADAR_COMMON_NAMESPACE_END
