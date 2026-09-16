#pragma once

#include <utility>
#include <type_traits>

#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

#ifndef __DOXYGEN__
namespace _defer_impl                           // NOLINT(readability-identifier-naming)
{
  template <typename T, typename = void>
  struct is_noarg_callable_t : std::false_type  // NOLINT(*-identifier-naming)
  {};

  template <typename T>
  struct is_noarg_callable_t<T, decltype(std::declval<T&>()())> : std::true_type
  {};

  template <typename T>
  struct returns_void_t  // NOLINT(*-identifier-naming)
    : std::is_same<void, decltype(std::declval<T&>()())>
  {};

  template <typename A, typename B, typename... C>
  struct and_t : and_t<A, and_t<B, C...>>  // NOLINT(*-identifier-naming)
  {};

  template <typename A, typename B>
  struct and_t<A, B> : std::conditional_t<A::value, B, A>
  {};

  template <typename T>
    struct is_proper_sg_callback_t : and_t< // NOLINT(*-identifier-naming)
                                       is_noarg_callable_t<T>,
                                       returns_void_t<T>,
                                       std::is_nothrow_invocable<T&>,
                                       std::is_nothrow_destructible<T>>
  {};

  template <
    typename Callback,
    typename = std::enable_if_t<is_proper_sg_callback_t<Callback>::value>>
  class scope_guard;

  template <typename Callback>
  _defer_impl::scope_guard<Callback> make_scope_guard(
    Callback&& callback
  ) noexcept(std::is_nothrow_constructible_v<Callback, Callback&&>);

  template <typename Callback>
  _defer_impl::scope_guard<Callback> finally(
    Callback&& callback
  ) noexcept(std::is_nothrow_constructible_v<Callback, Callback&&>);

  template <typename Callback>
  class [[nodiscard]] scope_guard<Callback> final
  {
   public:
    using callback_type = Callback;

    scope_guard(scope_guard&& other) noexcept(std::is_nothrow_constructible_v<Callback, Callback&&>)
      : m_callback(std::forward<Callback>(other.m_callback))
      , m_active(other.m_active)
    {
      other.m_active = false;
    }

    ~scope_guard() noexcept
    {
      if(this->m_active)
        this->m_callback();
    }

    void dismiss() noexcept { this->m_active = false; }

    scope_guard() = delete;
    scope_guard(scope_guard const&) = delete;
    scope_guard& operator=(scope_guard const&) = delete;
    scope_guard& operator=(scope_guard&&) = delete;

   private:
    explicit scope_guard(
      Callback&& callback  // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    ) noexcept(std::is_nothrow_constructible_v<Callback, Callback&&>)
      : m_callback(std::forward<Callback>(callback))
      , m_active(true)
    {}

    friend scope_guard<Callback> make_scope_guard<Callback>(
      Callback&&
    ) noexcept(std::is_nothrow_constructible_v<Callback, Callback&&>);

    friend scope_guard<Callback> finally<Callback>(
      Callback&&
    ) noexcept(std::is_nothrow_constructible_v<Callback, Callback&&>);

    Callback m_callback;
    bool m_active;
  };

  template <typename Callback>
  scope_guard<Callback> make_scope_guard(
    Callback&& callback
  ) noexcept(std::is_nothrow_constructible_v<Callback, Callback&&>)
  {
    return _defer_impl::scope_guard<Callback> {std::forward<Callback>(callback)};
  }

  template <typename Callback>
  scope_guard<Callback> finally(
    Callback&& callback
  ) noexcept(std::is_nothrow_constructible_v<Callback, Callback&&>)
  {
    return _defer_impl::scope_guard<Callback> {std::forward<Callback>(callback)};
  }
}  // namespace _defer_impl
#endif

/**
 * @brief Creates a scope guard that invokes a callback on destruction.
 * @details The callback is executed unless the returned guard is dismissed or moved from. The
 * callback must be callable with no arguments, return void, be nothrow-invocable, and be nothrow
 * destructible so that guard destruction is noexcept.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto file = open_file(path);
 * auto cleanup = common::make_scope_guard([&]() noexcept {
 *   close_file(file);
 * });
 *
 * write_file(file, bytes);
 * cleanup.dismiss(); // ownership was transferred elsewhere
 * @endcode
 *
 * @tparam Callback No-argument noexcept callback type.
 * @param callback Callback to execute when the guard leaves scope.
 * @return Active scope guard.
 * @ingroup core_functional
 * @see defer
 */
template <typename Callback>
[[nodiscard]] _defer_impl::scope_guard<Callback> make_scope_guard(Callback&& callback) noexcept(
  noexcept(_defer_impl::make_scope_guard<Callback>(std::forward<Callback>(callback)))
)
{
  return _defer_impl::make_scope_guard<Callback>(std::forward<Callback>(callback));
}

/**
 * @brief Alias for make_scope_guard() intended for deferred cleanup at the current scope exit.
 * @details Prefer this when the guard is purely local cleanup and does not need an explicit name.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto previous = set_temporary_mode(true);
 * auto restore = common::defer([&]() noexcept {
 *   set_temporary_mode(previous);
 * });
 * @endcode
 *
 * @tparam Callback No-argument noexcept callback type.
 * @param callback Callback to execute when the guard leaves scope.
 * @return Active scope guard.
 * @ingroup core_functional
 * @see make_scope_guard
 */
template <typename Callback>
[[nodiscard]] _defer_impl::scope_guard<Callback> defer(Callback&& callback) noexcept(
  noexcept(_defer_impl::finally<Callback>(std::forward<Callback>(callback)))
)
{
  return _defer_impl::finally<Callback>(std::forward<Callback>(callback));
}

RADAR_COMMON_NAMESPACE_END
