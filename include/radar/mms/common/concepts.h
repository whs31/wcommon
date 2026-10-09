#pragma once

#include <concepts>
#include <string_view>
#include <type_traits>

#include <radar/mms/common/detail/namespace.h>

/**
 * @defgroup core_concepts Concepts
 * @ingroup core
 * @brief Common C++20 concepts used by MMS core templates.
 */

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Concept that is true if T is a number.
 * @details Will yield `true` for any type that is an integral or floating point type,
 * except for `bool` and `std::nullptr_t`.
 * @note This concept will also yield `true` for references to numbers.
 * @tparam T Type to check.
 * @ingroup core_concepts
 * @see core_math
 */
template <typename T>
concept Number = (std::integral<std::remove_cvref_t<T>>
                  or std::floating_point<std::remove_cvref_t<T>>)
             and not std::is_same_v<std::remove_cvref_t<T>, bool>
             and not std::is_same_v<std::remove_cvref_t<T>, std::nullptr_t>;

/**
 * @brief Pointer concept.
 * @details Will yield `true` for raw pointer types.
 *
 * Note that this concept will be evaluated to `false` for references,
 * std::nullptr_t and smart pointers. Function pointers are raw pointer types
 * and therefore satisfy this concept.
 * @tparam T Type to check.
 * @ingroup core_concepts
 * @see SmartPointer
 * @see PointerLike
 */
template <typename T>
concept Pointer = std::is_pointer_v<T>;

/**
 * @brief Smart pointer concept.
 * @details Will yield `true` for any type that is a smart pointer.
 *
 * Requirements:
 *   - Can be dereferenced
 *   - Can be converted to `bool`
 *   - Operator `operator->` must return a pointer of the same type as the dereferenced pointer
 *   - Method `get` must return the same type as the dereferenced pointer
 * @tparam T Type to check.
 * @ingroup core_concepts
 * @see Pointer
 * @see PointerLike
 */
template <typename T>
concept SmartPointer = requires(T t) {
  { *t };
  { static_cast<bool>(t) };
  { t.operator->() } -> std::convertible_to<decltype(&*t)>;
  { t.get() } -> std::convertible_to<decltype(&*t)>;
};

/**
 * @brief Concept that will yield `true` if `T` is a pointer or smart pointer.
 * @tparam T Type to check.
 * @ingroup core_concepts
 * @see Pointer
 * @see SmartPointer
 * @see observer_ptr
 * @see propagate_const
 */
template <typename T>
concept PointerLike = Pointer<T> or SmartPointer<T>;

/**
 * @brief Describes a strongly typed physical quantity.
 * @details The primary template is empty, so a type is a Quantity only when it specializes this
 * trait. A specialization provides:
 *   - `static constexpr std::string_view si_symbol`, the symbol of the SI unit the quantity
 *     stores, e.g. `"m"`;
 *   - `static constexpr T from_si(double)`, which creates the quantity from its SI magnitude.
 *
 * RCommon specializes it for every type that derives from QuantityBase, which covers all RCommon
 * quantities. A project may derive its own quantity types from QuantityBase, or specialize this
 * trait for an unrelated type, to use them with the range checks, the math helpers and Curve.
 * @tparam T Quantity type.
 * @ingroup core_concepts
 * @see Quantity
 */
template <typename T>
struct QuantityTraits
{};

/**
 * @brief Concept that is true if T is a strongly typed physical quantity.
 * @details Requires a QuantityTraits specialization and an explicit conversion to the SI
 * magnitude as `double`. Yields `true` for references to quantities as well.
 * @tparam T Type to check.
 * @ingroup core_concepts
 * @see QuantityTraits
 * @see core_math
 */
template <typename T>
concept Quantity = std::is_class_v<std::remove_cvref_t<T>>
               and requires(std::remove_cvref_t<T> const value, double const magnitude) {
                     {
                       QuantityTraits<std::remove_cvref_t<T>>::si_symbol
                     } -> std::convertible_to<std::string_view>;
                     {
                       QuantityTraits<std::remove_cvref_t<T>>::from_si(magnitude)
                     } -> std::same_as<std::remove_cvref_t<T>>;
                     { static_cast<double>(value) } -> std::same_as<double>;
                   };

RADAR_COMMON_NAMESPACE_END
