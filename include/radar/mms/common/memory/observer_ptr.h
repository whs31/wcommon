#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include <radar/mms/common/config.h>
#include <radar/mms/common/check.h>
#include <radar/mms/common/concepts.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/memory/null_pointer_error.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetatype.h>
#endif

// NOLINTBEGIN(*-explicit-constructor)
RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief A smart pointer that holds a pointer to a value, but does not own it.
 * @details observer_ptr is a non-owning pointer, or observer. The observer stores a pointer to a
 * second object, known as the watched object. An observer_ptr may also have no watched object.
 *
 * An observer is not responsible in any way for the watched object; there is no inherent
 * relationship between an observer and the object it watches.
 *
 * It is intended as a near drop-in replacement for raw pointer types, with the advantage that, as
 * a vocabulary type, it indicates its intended use without need for detailed analysis by code
 * readers.
 *
 * @remarks This is a reimplementation of the LF TSv2 `std::observer_ptr` type.
 * @note `observer_ptr<T>` is declared as a Qt metatype template (if Qt is available), so registered
 * instantiations can be used in `QVariant`, `Q_PROPERTY`, signal and slot APIs.
 * @note Qt metatype registration is available only when rcommon is built with
 * `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * void inspect(observer_ptr<Widget> widget)
 * {
 *   if(widget)
 *     widget->dump();
 * }
 * @endcode
 *
 * @tparam T Type of the pointer
 * @ingroup core_memory
 * @see make_observer
 * @see Owner
 * @sa https://en.cppreference.com/w/cpp/experimental/observer_ptr
 * @version 2.1.31
 */
template <typename T>
class observer_ptr  // NOLINT(readability-identifier-naming)
{
 public:
  /**
   * @brief The element type.
   */
  using element_type = T;

  /**
   * @brief The pointer type.
   */
  using pointer = T*;

  /**
   * @brief The const pointer type.
   */
  using const_pointer = T const*;

  /**
   * @brief The reference type.
   */
  using reference = T&;

  /**
   * @brief The const reference type.
   */
  using const_reference = T const&;

  /**
   * @brief The value type. Equivalent to the element type without cv-qualifiers.
   */
  using value_type = std::remove_cv_t<T>;

  /**
   * @brief Constructs `nullptr` observer.
   */
  constexpr observer_ptr() noexcept
    : m_(nullptr)
  {}

  /**
   * @brief Constructs `nullptr` observer.
   */
  // ReSharper disable once CppNonExplicitConvertingConstructor
  constexpr observer_ptr(std::nullptr_t) noexcept
    : m_(nullptr)
  {}

  /**
   * @brief Constructs an observer from a pointer.
   * @param ptr Pointer to construct from.
   */
  // ReSharper disable once CppParameterMayBeConst
  // ReSharper disable once CppNonExplicitConvertingConstructor
  constexpr observer_ptr(pointer ptr) noexcept
    : m_(ptr)
  {}

  /**
   * @brief Constructs an observer from another observer.
   * @tparam U Type of the other observer. Must be convertible to `T*`.
   * @param other Observer to construct from.
   */
  template <typename U>
    requires std::convertible_to<U*, T*>
  // ReSharper disable once CppNonExplicitConvertingConstructor
  constexpr observer_ptr(observer_ptr<U> const& other) noexcept
    : m_(other.get())
  {}

  /**
   * @brief Constructs an observer from a unique pointer.
   * @tparam U Underlying type of the unique pointer. Must be convertible to `T*`.
   * @param other Unique pointer to construct from.
   */
  template <typename U>
    requires std::convertible_to<U*, T*>
  // ReSharper disable once CppNonExplicitConvertingConstructor
  constexpr observer_ptr(std::unique_ptr<U> const& other) noexcept
    : m_(other.get())
  {}

  /**
   * @brief Returns the watched pointer.
   * @return The watched pointer.
   */
  [[nodiscard]] pointer get() const noexcept { return this->m_; }

  /**
   * @brief Returns the constant reference to the watched object.
   * @return The reference to the watched object.
   * @throws NullPointerError if the underlying pointer is `nullptr`
   */
  [[nodiscard]] const_reference ref() const
  {
    RADAR_SOFT_CHECK(this->m_ != nullptr, NullPointerError);  // NOLINT(*-avoid-do-while)
    return **this;
  }

  /**
   * @brief Returns the reference to the watched object.
   * @return The reference to the watched object.
   * @throws NullPointerError if the underlying pointer is `nullptr`
   */
  [[nodiscard]] reference ref()
  {
    RADAR_SOFT_CHECK(this->m_ != nullptr, NullPointerError);  // NOLINT(*-avoid-do-while)
    return **this;
  }

  /**
   * @brief Returns the dereferenced watched object.
   * @return The reference to the watched object.
   * @throws NullPointerError if the underlying pointer is `nullptr`.
   */
  [[nodiscard]] reference operator*() const
  {
    RADAR_SOFT_CHECK(this->m_ != nullptr, NullPointerError);  // NOLINT(*-avoid-do-while)
    return *this->m_;
  }

  /**
   * @brief Returns the pointer to the watched object.
   * @return The pointer to the watched object.
   * @throws NullPointerError if the underlying pointer is `nullptr`
   */
  [[nodiscard]] constexpr pointer operator->() const
  {
    RADAR_SOFT_CHECK(this->m_ != nullptr, NullPointerError);  // NOLINT(*-avoid-do-while)
    return this->m_;
  }

  /**
   * @brief Returns `true` if the watched pointer is not `nullptr`.
   */
  [[nodiscard]] explicit operator bool() const noexcept { return this->m_ != nullptr; }

  /**
   * @brief Returns the watched pointer.
   * @return The watched pointer.
   */
  [[nodiscard]] explicit operator pointer() const noexcept { return this->m_; }

  /**
   * @brief Returns the watched pointer and resets the watched pointer to `nullptr`.
   * @return The watched pointer.
   */
  constexpr pointer release() noexcept
  {
    auto* ptr = this->m_;
    this->m_ = nullptr;
    return ptr;
  }

  /**
   * @brief Resets the watched pointer to `nullptr` or the new watched pointer.
   * @param ptr The new watched pointer. Defaults to `nullptr`.
   */
  constexpr void reset(pointer ptr = nullptr) noexcept { this->m_ = ptr; }

  /**
   * @brief Swaps two watched pointers.
   * @param other The other observer.
   */
  constexpr void swap(observer_ptr& other) noexcept { std::swap(this->m_, other.m_); }

 private:
  pointer m_;
};

/**
 * @brief Compares two observers for pointer equality.
 * @relates observer_ptr
 */
template <typename T, typename U>
constexpr bool operator==(observer_ptr<T> const& lhs, observer_ptr<U> const& rhs) noexcept
{
  return lhs.get() == rhs.get();
}

/**
 * @brief Compares an observer with `nullptr` for pointer equality.
 * @relates observer_ptr
 */
template <typename T>
constexpr bool operator==(observer_ptr<T> const& lhs, std::nullptr_t) noexcept
{
  return lhs.get() == nullptr;
}

/**
 * @brief Compares `nullptr` with an observer for pointer equality.
 * @relates observer_ptr
 */
template <typename T>
constexpr bool operator==(std::nullptr_t, observer_ptr<T> const& rhs) noexcept
{
  return rhs == nullptr;
}

/**
 * @brief Compares two observers for pointer inequality.
 * @relates observer_ptr
 */
template <typename T, typename U>
constexpr bool operator!=(observer_ptr<T> const& lhs, observer_ptr<U> const& rhs) noexcept
{
  return lhs.get() != rhs.get();
}

/**
 * @brief Compares an observer with `nullptr` for pointer inequality.
 * @relates observer_ptr
 */
template <typename T>
constexpr bool operator!=(observer_ptr<T> const& lhs, std::nullptr_t) noexcept
{
  return lhs.get() != nullptr;
}

/**
 * @brief Compares `nullptr` with an observer for pointer inequality.
 * @relates observer_ptr
 */
template <typename T>
constexpr bool operator!=(std::nullptr_t, observer_ptr<T> const& rhs) noexcept
{
  return rhs != nullptr;
}

/**
 * @brief Orders two observers by their stored pointer values.
 * @relates observer_ptr
 */
template <typename T, typename U>
constexpr bool operator<(observer_ptr<T> const& lhs, observer_ptr<U> const& rhs) noexcept
{
  return std::less<> {}(lhs.get(), rhs.get());
}

/**
 * @brief Orders two observers by their stored pointer values.
 * @relates observer_ptr
 */
template <typename T, typename U>
constexpr bool operator>(observer_ptr<T> const& lhs, observer_ptr<U> const& rhs) noexcept
{
  return rhs < lhs;
}

/**
 * @brief Orders two observers by their stored pointer values.
 * @relates observer_ptr
 */
template <typename T, typename U>
constexpr bool operator<=(observer_ptr<T> const& lhs, observer_ptr<U> const& rhs) noexcept
{
  return not(rhs < lhs);
}

/**
 * @brief Orders two observers by their stored pointer values.
 * @relates observer_ptr
 */
template <typename T, typename U>
constexpr bool operator>=(observer_ptr<T> const& lhs, observer_ptr<U> const& rhs) noexcept
{
  return not(lhs < rhs);
}

/**
 * @brief Creates an observer from a raw pointer.
 * @tparam T Watched object type.
 * @param ptr Pointer to observe.
 * @return Non-owning observer for `ptr`.
 * @ingroup core_memory
 * @relates observer_ptr
 */
template <typename T>
[[nodiscard]] constexpr observer_ptr<T> make_observer(T* ptr) noexcept
{
  return observer_ptr<T>(ptr);
}

/**
 * @brief Explicitly converts a reference or pointer-like object to an `observer_ptr`.
 * @details This utility function allows for the creation of an `observer_ptr` from an
 * existing object without changing its cv-qualification.
 *
 * It is particularly useful when you need to bridge between raw references and
 * observer-based APIs while preserving const-correctness.
 *
 * @section usage_example Example
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * void process(observer_ptr<Data> ptr);
 *
 * Data d;
 * process(observer_cast<Data>(d));
 * @endcode
 *
 * @tparam To The target type to be watched by the resulting `observer_ptr`.
 * @tparam From The type of the source object.
 * @param from The object to observe.
 * @return An `observer_ptr<To>` pointing to the address of @p from.
 * @relates observer_ptr
 * @ingroup core_memory
 */
template <typename To, typename From>
  requires PointerLike<std::remove_cv_t<From>>
        && requires(From& value) { static_cast<To*>(std::to_address(value)); }
[[nodiscard]] observer_ptr<To> observer_cast(From& from)
{
  return make_observer(static_cast<To*>(std::to_address(from)));
}

template <typename To, typename From>
  requires(not PointerLike<std::remove_cv_t<From>>)
       && requires(From& value) { static_cast<To*>(std::addressof(value)); }
[[nodiscard]] observer_ptr<To> observer_cast(From& from)
{
  return make_observer(static_cast<To*>(std::addressof(from)));
}

RADAR_COMMON_NAMESPACE_END
// NOLINTEND(*-explicit-constructor)

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE_TEMPLATE_1ARG(radar::mms::common::observer_ptr)
#endif
