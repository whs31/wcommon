#pragma once

#include <radar/mms/common/memory/observer_ptr.h>
#include <radar/mms/common/detail/namespace.h>

/**
 * @defgroup core_adapters Adapters
 * @ingroup core
 * @brief Non-owning adapter base utilities.
 * @see Adapter
 * @see observer_ptr
 */

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Non-owning base for objects that adapt another object.
 * @details Adapter stores an observer_ptr to an externally-owned adaptee and exposes access to it
 * through adaptee(). The adapter never extends the lifetime of the adaptee; callers
 * must ensure the adapted object outlives the adapter.
 *
 * Use `Adapter<T>` for mutable adaptation and `Adapter<T const>` for read-only adaptation.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * class WidgetPresenter : public Adapter<Widget>
 * {
 *  public:
 *   using Adapter::Adapter;
 *
 *   void refresh()
 *   {
 *     this->adaptee().reload();
 *   }
 * };
 *
 * auto presenter = WidgetPresenter(widget);
 * presenter.refresh();
 * @endcode
 *
 * @tparam T Adapted object type. May be const-qualified for read-only adapters.
 * @ingroup core_adapters
 * @see observer_ptr
 * @see make_observer
 */
template <typename T>
class Adapter
{
 public:
  /**
   * @brief Creates an adapter from a non-owning observer.
   * @param adaptee Object observer. Must be non-null before adaptee() is called.
   */
  explicit constexpr Adapter(observer_ptr<T> const adaptee) noexcept
    : m_adaptee(adaptee)
  {}

  /**
   * @brief Creates an adapter from an object reference.
   * @param adaptee Object to adapt. The object must outlive this adapter.
   */
  explicit constexpr Adapter(T& adaptee) noexcept
    : m_adaptee(make_observer(&adaptee))
  {}

  /**
   * @brief Returns true if this adapter currently observes an adaptee.
   */
  [[nodiscard]] bool has_adaptee() const { return static_cast<bool>(this->m_adaptee); }

  /**
   * @brief Returns the observed adaptee pointer.
   */
  [[nodiscard]] observer_ptr<T> adaptee_ptr() const { return this->m_adaptee; }

  /**
   * @brief Returns const access to the adapted object.
   * @invariant has_adaptee() is true.
   */
  [[nodiscard]] T const& adaptee() const { return this->m_adaptee.ref(); }

  /**
   * @brief Returns mutable access to the adapted object.
   * @invariant has_adaptee() is true.
   */
  [[nodiscard]] T& adaptee() { return this->m_adaptee.ref(); }

  /**
   * @brief Promotes a `const` pointer to non-const one.
   * @warning Use with caution.
   */
  template <typename U>
  [[nodiscard]] static U* promote(U const* ptr)
  {
    return const_cast<U*>(ptr);  // NOLINT(*-pro-type-const-cast)
  }

 private:
  observer_ptr<T> m_adaptee;
};

RADAR_COMMON_NAMESPACE_END
