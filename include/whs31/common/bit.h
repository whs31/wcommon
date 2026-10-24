#pragma once

#include <cstdint>
#include <bit>

#include <whs31/common/detail/namespace.h>

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Returns the bit representation of a trivially copyable value.
 * @tparam T Value type, whose size must match a supported unsigned integer type.
 * @param value Value to inspect.
 * @return Unsigned integer containing the object's bit representation.
 * @ingroup core_math
 */
template <typename T>
  requires(
    std::is_trivially_copyable_v<T>
    and (sizeof(T) == sizeof(std::uint8_t) or sizeof(T) == sizeof(std::uint16_t) or sizeof(T) == sizeof(std::uint32_t) or sizeof(T) == sizeof(std::uint64_t))
  )
[[nodiscard]] constexpr auto bits_of(T const value) noexcept
{
  if constexpr(sizeof(T) == sizeof(std::uint8_t))
    return std::bit_cast<std::uint8_t>(value);
  else if constexpr(sizeof(T) == sizeof(std::uint16_t))
    return std::bit_cast<std::uint16_t>(value);
  else if constexpr(sizeof(T) == sizeof(std::uint32_t))
    return std::bit_cast<std::uint32_t>(value);
  else
    return std::bit_cast<std::uint64_t>(value);
}

WHS31_COMMON_NAMESPACE_END
