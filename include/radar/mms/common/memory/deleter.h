#pragma once

#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Function-pointer deleter adapter.
 * @details Adapts a C-style cleanup function such as `std::fclose` to the
 * callable form expected by RAII wrappers like `std::unique_ptr`.
 *
 * @code {.cpp}
 * using FilePtr = std::unique_ptr<std::FILE, radar::mms::Deleter<int, std::FILE*, std::fclose>>;
 * auto file = FilePtr(std::fopen(path.c_str(), "rb"));
 * @endcode
 *
 * @tparam R Return type of the cleanup function.
 * @tparam T Argument type accepted by the cleanup function.
 * @tparam F Cleanup function pointer.
 * @ingroup core_memory
 * @see Owner
 */
template <typename R, typename T, R (*F)(T)>
struct Deleter
{
  /**
   * @brief Invokes the configured cleanup function.
   * @param ref Resource handle to release.
   */
  void operator()(T ref) const noexcept(noexcept((*F)(ref))) { (*F)(ref); }
};

RADAR_COMMON_NAMESPACE_END
