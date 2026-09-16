#pragma once

#include <string_view>

#include <radar/mms/common/error.h>
#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Error thrown when an observer is dereferenced while holding `nullptr`.
 * @ingroup core_errors
 * @ingroup core_memory
 */
class NullPointerError final : public Error
{
 public:
  /**
   * @brief Constructs a null pointer dereference error.
   */
  NullPointerError()
    : Error("null pointer dereference")
  {}

  /**
   * @brief Constructs a null pointer dereference error with additional details.
   */
  explicit NullPointerError(std::string_view const message)
    : Error("null pointer dereference ({})", message)
  {}
};

RADAR_COMMON_NAMESPACE_END
