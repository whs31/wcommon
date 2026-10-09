#pragma once

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

RADAR_COMMON_NAMESPACE_END
