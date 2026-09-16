#pragma once

#include <concepts>
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

RADAR_COMMON_NAMESPACE_END
