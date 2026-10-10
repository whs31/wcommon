#pragma once

#include <concepts>
#include <cstddef>
#include <optional>
#include <string_view>

#include <glaze/core/common.hpp>
#include <glaze/core/context.hpp>
#include <glaze/core/custom.hpp>

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
 * with `glz::error_code::constraint_violated`. These alternative units are a glaze extension: the
 * nlohmann adapters read the SI unit only.
 *
 * `std::chrono` durations and time points use glaze's built-in support. Durations serialize as
 * their count, as with the nlohmann adapters, but `system_clock` time points serialize as ISO 8601
 * strings.
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
   * @brief `glz::custom` member that reads and writes the SI magnitude of a quantity.
   */
  template <DerivedQuantity T>
  inline constexpr auto si_magnitude = glz::custom<
    [](T& value, double const magnitude) { value = QuantityTraits<T>::from_si(magnitude); },
    [](T const& value) { return static_cast<double>(value); }>;

  /**
   * @brief One unit a quantity may be read in: the magnitude under its key, if the document has
   * it, and the factory converting that magnitude.
   */
  template <DerivedQuantity T>
  struct UnitMagnitude
  {
    std::optional<double> magnitude;
    T (*from)(double) = nullptr;
  };

  /**
   * @brief Assigns @p value from the one unit of @p units the document holds, failing the read
   * with `constraint_violated` if it holds none or several.
   */
  template <DerivedQuantity T, std::same_as<UnitMagnitude<T>>... Units>
  void read_in_unit(T& value, glz::context& ctx, Units const... units) noexcept
  {
    auto const present = (std::size_t {units.magnitude.has_value()} + ...);
    if(present != 1) {
      reject(ctx, "a quantity must hold its magnitude in exactly one unit");
      return;
    }
    auto const assign = [&value](UnitMagnitude<T> const& unit) {
      if(unit.magnitude.has_value())
        value = unit.from(*unit.magnitude);
    };
    (assign(units), ...);
  }
}  // namespace _glaze_impl

RADAR_COMMON_NAMESPACE_END
