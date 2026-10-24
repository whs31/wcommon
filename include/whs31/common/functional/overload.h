#pragma once

#include <whs31/common/detail/namespace.h>

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Selects a non-const member-function overload by argument list.
 * @tparam Args Argument types of the overload to select.
 * @ingroup core_functional
 * @see const_overload
 * @see overload
 */
template <typename... Args>
struct non_const_overload_t  // NOLINT(*-identifier-naming)
{
  template <typename R, typename T>
  auto constexpr operator()(R (T::*ptr)(Args...)) const noexcept -> decltype(ptr)
  {
    return ptr;
  }

  template <typename R, typename T>
  static auto constexpr of(R (T::*ptr)(Args...)) noexcept -> decltype(ptr)
  {
    return ptr;
  }
};

/**
 * @brief Selects a const member-function overload by argument list.
 * @tparam Args Argument types of the overload to select.
 * @ingroup core_functional
 * @see non_const_overload
 * @see overload
 */
template <typename... Args>
struct const_overload_t  // NOLINT(*-identifier-naming)
{
  template <typename R, typename T>
  auto constexpr operator()(R (T::*ptr)(Args...) const) const noexcept -> decltype(ptr)
  {
    return ptr;
  }

  template <typename R, typename T>
  static auto constexpr of(R (T::*ptr)(Args...) const) noexcept -> decltype(ptr)
  {
    return ptr;
  }
};

/**
 * @brief Selects a free-function or member-function overload by argument list.
 * @details Useful when passing overloaded functions to templates or signal/slot APIs.
 *
 * @code {.cpp}
 * using namespace whs31;
 *
 * auto as_int = common::overload<int>(parse);
 * auto value = as_int("42");
 *
 * auto setter = common::non_const_overload<int>(&Widget::set_value);
 * (widget.*setter)(42);
 * @endcode
 *
 * @tparam Args Argument types of the overload to select.
 * @ingroup core_functional
 * @see const_overload
 * @see non_const_overload
 */
template <typename... Args>
struct overload_t : const_overload_t<Args...>,  // NOLINT(*-identifier-naming)
                    non_const_overload_t<Args...>
{
  using const_overload_t<Args...>::of;
  using const_overload_t<Args...>::operator();
  using non_const_overload_t<Args...>::of;
  using non_const_overload_t<Args...>::operator();

  template <typename R>
  auto constexpr operator()(R (*ptr)(Args...)) const noexcept -> decltype(ptr)
  {
    return ptr;
  }

  template <typename R>
  static auto constexpr of(R (*ptr)(Args...)) noexcept -> decltype(ptr)
  {
    return ptr;
  }
};

/**
 * @brief Inline selector for overloaded free functions and member functions.
 * @ingroup core_functional
 */
template <typename... Args>
constexpr inline overload_t<Args...> overload = {};

/**
 * @brief Inline selector for overloaded const member functions.
 * @ingroup core_functional
 */
template <typename... Args>
constexpr inline const_overload_t<Args...> const_overload = {};

/**
 * @brief Inline selector for overloaded non-const member functions.
 * @ingroup core_functional
 */
template <typename... Args>
constexpr inline non_const_overload_t<Args...> non_const_overload = {};

WHS31_COMMON_NAMESPACE_END
