#pragma once

#include <utility>

#include <radar/mms/common/concepts.h>
#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Wrapper that propagates constness through pointer-like storage.
 * @details `propagate_const<T>` publicly derives from the wrapped pointer-like
 * type and overrides dereference, arrow and `get()` accessors so const wrapper
 * instances expose only const access to the pointee.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * class WidgetOwner
 * {
 *   propagate_const<std::unique_ptr<Widget>> m_widget;
 * };
 * @endcode
 *
 * @tparam T Pointer-like storage type. Must provide `element_type`, `operator*`,
 * `operator->`, `get()` and conversion to `bool`.
 * @ingroup core_memory
 * @see Pimpl
 * @see SharedPimpl
 */
template <typename T>
class propagate_const : public T  // NOLINT(readability-identifier-naming)
{
 public:
  /**
   * @brief Inherits assignment operators from the wrapped pointer-like type.
   */
  using T::operator=;

  /**
   * @brief Constructs an empty wrapper.
   */
  propagate_const() = default;

  /**
   * @brief Constructs the wrapped pointer-like type in place.
   * @tparam Args Constructor argument types.
   * @param args Arguments forwarded to the wrapped type constructor.
   */
  template <typename... Args>
  inline propagate_const(Args&&... args)  // NOLINT
    : T(std::forward<Args>(args)...)
  {}

  /**
   * @brief Returns a mutable reference to the pointee.
   */
  [[nodiscard]] T::element_type& operator*() { return T::operator*(); }

  /**
   * @brief Returns a const reference to the pointee.
   */
  [[nodiscard]] T::element_type const& operator*() const { return T::operator*(); }

  /**
   * @brief Returns a mutable pointer to the pointee.
   */
  [[nodiscard]] T::element_type* operator->() { return T::operator->(); }

  /**
   * @brief Returns a const pointer to the pointee.
   */
  [[nodiscard]] T::element_type const* operator->() const { return T::operator->(); }

  /**
   * @brief Returns the mutable stored pointer.
   */
  [[nodiscard]] T::element_type* get() { return T::get(); }

  /**
   * @brief Returns the stored pointer as a const pointer.
   */
  [[nodiscard]] T::element_type const* get() const { return T::get(); }

  /**
   * @brief Returns true if the wrapped pointer-like value is non-null.
   */
  [[nodiscard]] operator bool() const noexcept  // NOLINT
  {
    return T::operator bool();
  }
};

RADAR_COMMON_NAMESPACE_END
