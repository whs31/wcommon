#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <memory>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <variant>

#include <glaze/core/common.hpp>
#include <glaze/core/context.hpp>
#include <glaze/core/custom.hpp>
#include <glaze/core/read.hpp>

#include <radar/mms/common/concepts.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/quantity.h>

/**
 * @defgroup core_glaze Glaze adapters
 * @ingroup core
 * @brief `glz::meta` specializations for RCommon types, provided by `radar::mms::common::glaze`.
 * @details The adapters produce the same JSON as the nlohmann adapters under
 * `radar/mms/common/json/`, so documents written by one are read by the other. They are
 * format-agnostic and work with every glaze format, e.g. JSON and BEVE.
 *
 * Missing keys follow glaze's `error_on_missing_keys` option, as for any reflected struct. Pass
 * `glz::opts {.error_on_missing_keys = true}` for the strictness of the nlohmann adapters. Values
 * that a type rejects, e.g. an invalid UUID string, fail with
 * `glz::error_code::constraint_violated`.
 *
 * Quantities are written in their SI unit. Reading also accepts any other unit the quantity has a
 * factory for, e.g. `{"ampere_hours": 52}` for a Charge or `{"celsius": 15}` for a Temperature,
 * which keeps hand-written documents such as configuration files in their natural units. A
 * quantity object must hold exactly one unit; an empty object, or one with several units, fails
 * with `glz::error_code::constraint_violated`.
 *
 * In a text format, JSON or TOML, a quantity may also be a string spelled like its C++ literal
 * without the underscore: `"400hz"` for `400_hz`, `"-1.5deg"` for `-1.5_deg`, `"52ah"` for
 * `52_ah`. The magnitude is any floating-point number and is followed by the suffix immediately;
 * an unknown suffix fails with `glz::error_code::constraint_violated` and a message listing the
 * suffixes of the quantity. Binary formats such as BEVE read the object only.
 *
 * The alternative units and the literal strings are a glaze extension: the nlohmann adapters read
 * the SI unit only.
 *
 * `std::chrono` durations serialize as their count, as with the nlohmann adapters; with
 * `glaze/chrono.h` they also read from a unit key or a literal string, `{"milliseconds": 7.5}` or
 * `"7.5ms"`. Time points use glaze's built-in support: `system_clock` ones serialize as ISO 8601
 * strings, unlike with the nlohmann adapters.
 */

RADAR_COMMON_NAMESPACE_BEGIN

namespace _glaze_impl  // NOLINT(readability-identifier-naming)
{
  /**
   * @brief Fails the current glaze read with `constraint_violated` and a static @p message.
   */
  inline void reject(glz::context& ctx, std::string_view const message) noexcept
  {
    ctx.error = glz::error_code::constraint_violated;
    ctx.custom_error_message = message;
  }

  /**
   * @brief One unit key a quantity may be read in: the member of @p Fields holding the magnitude
   * under that key, if the document has it, and the factory converting that magnitude.
   */
  template <DerivedQuantity T, typename Fields>
  struct FieldUnit
  {
    std::optional<double> Fields::* field = nullptr;
    T (*from)(double) = nullptr;
  };

  /**
   * @brief One unit a literal string may name: the suffix of the C++ literal, without its
   * underscore, and the factory the literal calls.
   */
  template <DerivedQuantity T>
  struct LiteralUnit
  {
    std::string_view suffix;
    T (*from)(double) = nullptr;
  };

  template <DerivedQuantity T, typename Fields>
  [[nodiscard]] constexpr FieldUnit<T, Fields> field_unit(
    std::optional<double> Fields::* const field,
    T (*const from)(double)
  ) noexcept
  {
    return {
      .field = field,
      .from = from,
    };
  }

  template <DerivedQuantity T>
  [[nodiscard]] constexpr LiteralUnit<T> literal_unit(
    std::string_view const suffix,
    T (*const from)(double)
  ) noexcept
  {
    return {
      .suffix = suffix,
      .from = from,
    };
  }

  /**
   * @brief Every way a quantity may be read: its unit keys, its literal suffixes, and the message
   * for a literal with an unknown suffix.
   */
  template <DerivedQuantity T, typename Fields, std::size_t FieldCount, std::size_t LiteralCount>
  struct QuantityReading
  {
    using quantity_type = T;
    using fields_type = Fields;

    std::array<FieldUnit<T, Fields>, FieldCount> fields;
    std::array<LiteralUnit<T>, LiteralCount> literals;
    std::string_view unknown_suffix;
  };

  /**
   * @brief What a quantity is read from: the object of its unit keys or, in a text format, a
   * literal string such as `"400hz"`.
   */
  template <typename Fields>
  class QuantityInput
  {
   public:
    // note: [whs31 : 2026-10-10]
    // not an aggregate, so that glaze reads it through the specialization below rather than by
    // reflection
    [[nodiscard]] std::variant<Fields, std::string>& value() noexcept { return this->m_value; }

    [[nodiscard]] std::variant<Fields, std::string> const& value() const noexcept
    {
      return this->m_value;
    }

   private:
    std::variant<Fields, std::string> m_value;
  };

  /**
   * @brief Assigns @p value from the one unit of @p units that @p fields holds, failing the read
   * with `constraint_violated` if it holds none or several.
   */
  template <DerivedQuantity T, typename Fields, std::size_t FieldCount>
  void read_fields(
    T& value,
    glz::context& ctx,
    Fields const& fields,
    std::array<FieldUnit<T, Fields>, FieldCount> const& units
  ) noexcept
  {
    auto const present = std::ranges::count_if(units, [&fields](auto const& unit) {
      return (fields.*unit.field).has_value();
    });
    if(present != 1) {
      reject(ctx, "a quantity must hold its magnitude in exactly one unit");
      return;
    }
    for(auto const& unit : units)
      if(auto const& magnitude = fields.*unit.field; magnitude.has_value())
        value = unit.from(*magnitude);
  }

  /**
   * @brief Assigns @p value from a literal string: the magnitude immediately followed by the
   * suffix of one of @p units, e.g. `"400hz"` or `"-1.5e3m"`.
   * @details Fails the read with `constraint_violated` if the string does not start with a number,
   * or with @p unknown_suffix if the rest of it is not one of the suffixes.
   */
  template <DerivedQuantity T, std::size_t LiteralCount>
  void read_literal(
    T& value,
    glz::context& ctx,
    std::string_view const literal,
    std::array<LiteralUnit<T>, LiteralCount> const& units,
    std::string_view const unknown_suffix
  ) noexcept
  {
    auto magnitude = 0.0;
    auto const* const last = std::to_address(literal.end());
    auto const [suffix_begin, error] = std::
      from_chars(std::to_address(literal.begin()), last, magnitude);
    if(error != std::errc {}) {
      reject(ctx, "a quantity literal must start with its magnitude");
      return;
    }
    auto const suffix = std::string_view(suffix_begin, last);
    auto const unit = std::ranges::find(units, suffix, &LiteralUnit<T>::suffix);
    if(unit == units.end()) {
      reject(ctx, unknown_suffix);
      return;
    }
    value = unit->from(magnitude);
  }

  /**
   * @brief Assigns @p value from @p input as @p reading allows.
   */
  template <DerivedQuantity T, typename Fields, std::size_t FieldCount, std::size_t LiteralCount>
  void read_quantity(
    T& value,
    glz::context& ctx,
    QuantityInput<Fields> const& input,
    QuantityReading<T, Fields, FieldCount, LiteralCount> const& reading
  ) noexcept
  {
    if(auto const* const literal = std::get_if<std::string>(&input.value()))
      read_literal(value, ctx, *literal, reading.literals, reading.unknown_suffix);
    else if(auto const* const fields = std::get_if<Fields>(&input.value()))
      read_fields(value, ctx, *fields, reading.fields);
  }

  /**
   * @brief `glz::meta` value of a quantity: reads it as @p Reading allows, and writes the object
   * of its first unit key, the SI unit.
   */
  template <auto const& Reading>
  struct QuantityAdapter
  {
    // note: [whs31 : 2026-10-10]
    // readability-redundant-parentheses misreads every use of a reference template parameter
    static auto constexpr const& reading = Reading;  // NOLINT(readability-redundant-parentheses)

    using quantity_type = std::remove_cvref_t<decltype(reading)>::quantity_type;
    using fields_type = std::remove_cvref_t<decltype(reading)>::fields_type;

    static auto constexpr value = glz::custom<
      [](quantity_type& value, QuantityInput<fields_type> const& input, glz::context& ctx) {
        read_quantity(value, ctx, input, reading);
      },
      [](quantity_type const& value) {
        auto fields = fields_type {};
        fields.*reading.fields.front().field = static_cast<double>(value);
        return fields;
      }>;
  };
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END

/**
 * @brief Reads a QuantityInput: a text format takes either the object of unit keys or a string,
 * by the first character of the value; any other format takes the object only, so a binary
 * document reads back what the adapter writes.
 */
template <std::uint32_t Format, typename Fields>
struct glz::from<Format, radar::mms::common::_glaze_impl::QuantityInput<Fields>>
{
  template <auto Opts>
  static void op(auto&& input, is_context auto&& ctx, auto&& it, auto end)
  {
    if constexpr(Format == glz::JSON or Format == glz::TOML)
      glz::from<Format, std::variant<Fields, std::string>>::template op<
        Opts>(input.value(), ctx, it, end);
    else
      glz::from<Format, Fields>::template op<
        Opts>(input.value().template emplace<Fields>(), ctx, it, end);
  }
};
