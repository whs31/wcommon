#pragma once

#include <cstdint>
#include <string_view>

#include <radar/mms/common/detail/namespace.h>

/**
 * @defgroup core_text Text Helpers
 * @ingroup core
 * @brief ASCII character and hexadecimal conversion helpers.
 * @see Uuid
 * @see Version
 */

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Converts an ASCII uppercase letter to lowercase.
 * @ingroup core_text
 */
[[nodiscard]] constexpr char to_lower(char const c) noexcept
{
  return (c >= 'A' and c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
}

/**
 * @brief Returns true if @p c is an ASCII decimal digit.
 * @ingroup core_text
 * @see to_digit
 */
[[nodiscard]] constexpr bool is_digit(char const c) noexcept { return c >= '0' and c <= '9'; }

/**
 * @brief Returns true if @p c is an ASCII space.
 * @ingroup core_text
 */
[[nodiscard]] constexpr bool is_space(char const c) noexcept { return c == ' '; }

/**
 * @brief Returns true if @p c is one of the comparison operator characters.
 * @ingroup core_text
 */
[[nodiscard]] constexpr bool is_operator(char const c) noexcept
{
  return c == '<' or c == '>' or c == '=';
}

/**
 * @brief Returns true if @p c is a dot.
 * @ingroup core_text
 */
[[nodiscard]] constexpr bool is_dot(char const c) noexcept { return c == '.'; }

/**
 * @brief Returns true if @p c is a logical-or pipe character.
 * @ingroup core_text
 */
[[nodiscard]] constexpr bool is_logical_or(char const c) noexcept { return c == '|'; }

/**
 * @brief Returns true if @p c is a hyphen.
 * @ingroup core_text
 */
[[nodiscard]] constexpr bool is_hyphen(char const c) noexcept { return c == '-'; }

/**
 * @brief Returns true if @p c is an ASCII letter.
 * @ingroup core_text
 */
[[nodiscard]] constexpr bool is_letter(char const c) noexcept
{
  return (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z');
}

/**
 * @brief Returns true if @p c is an ASCII hexadecimal digit.
 * @ingroup core_text
 * @see hex_to_char
 * @see encode_hex
 */
[[nodiscard]] constexpr bool is_hex(char const c) noexcept
{
  return is_digit(c) or (c >= 'a' and c <= 'f') or (c >= 'A' and c <= 'F');
}

/**
 * @brief Converts an ASCII decimal digit to its numeric value.
 * @pre @p c must satisfy is_digit().
 * @ingroup core_text
 * @see is_digit
 */
[[nodiscard]] constexpr std::uint16_t to_digit(char const c) noexcept
{
  return static_cast<std::uint16_t>(c - '0');
}

/**
 * @brief Converts an ASCII hexadecimal digit to its numeric nibble value.
 * @return A value in `[0, 15]`, or `static_cast<unsigned char>(-1)` for invalid input.
 * @ingroup core_text
 * @see is_hex
 * @see hex_byte_join
 */
[[nodiscard]] constexpr unsigned char hex_to_char(char const c)
{
  if(c >= '0' and c <= '9')
    return static_cast<unsigned char>(c - '0');
  if(c >= 'a' and c <= 'f')
    return static_cast<unsigned char>(c - 'a' + 10);
  if(c >= 'A' and c <= 'F')
    return static_cast<unsigned char>(c - 'A' + 10);
  return static_cast<unsigned char>(-1);
}

/**
 * @brief Encodes a numeric nibble as a lowercase hexadecimal character.
 * @pre @p value must be less than 16.
 * @ingroup core_text
 * @see hex_to_char
 */
[[nodiscard]] constexpr char encode_hex(std::uint8_t const value) noexcept
{
  return static_cast<char>((value < 10) ? ('0' + value) : ('a' + value - 10));
}

/**
 * @brief Decodes two hexadecimal characters from a string view into one byte.
 * @pre @p idx and `idx + 1` must be valid positions in @p str, and both characters must satisfy
 * is_hex().
 * @ingroup core_text
 * @see Uuid
 */
[[nodiscard]] constexpr std::uint8_t hex_byte_join(std::string_view const str, int const idx)
{
  return ((hex_to_char(str[idx]) << 4) + hex_to_char(str[idx + 1]));  // NOLINT
}

RADAR_COMMON_NAMESPACE_END
