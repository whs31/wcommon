#pragma once

#include <utility>
#include <type_traits>
#include <functional>
#include <optional>
#include <concepts>

#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

#ifndef __DOXYGEN__
namespace _lazy_impl  // NOLINT(readability-identifier-naming)
{
  template <std::invocable F>
  class lazy          // NOLINT(*-identifier-naming)
  {
   public:
    using result_type = std::invoke_result_t<F>;

    explicit lazy(F&& f)
      : m_f(std::move(f))
    {}

    explicit lazy(F const& f)
      : m_f(f)
    {}

    ~lazy() = default;
    lazy(lazy const&) = delete;
    lazy& operator=(lazy const&) = delete;
    lazy& operator=(lazy&&) noexcept = delete;

    lazy(lazy&& m) noexcept(
      std::is_nothrow_move_constructible_v<std::optional<result_type>>
      and std::is_nothrow_move_constructible_v<F>
    )
      : m_value(std::move(m.m_value))
      , m_f(std::move(m.m_f))
    {}

    [[nodiscard]] result_type const& operator()() const
    {
      this->eval();
      return *this->m_value;
    }

    [[nodiscard]] result_type& operator()()
    {
      this->eval();
      return *this->m_value;
    }

   private:
    void eval() const
    {
      if(not this->m_value)
        this->m_value = std::invoke(this->m_f);
    }

    mutable std::optional<result_type> m_value;
    mutable F m_f;
  };
}  // namespace _lazy_impl
#endif

/**
 * @brief Creates a lazily evaluated value from a nullary callable.
 * @details The callable is invoked the first time the returned object is called. The result is
 * stored internally, and subsequent calls return a reference to the cached value without invoking
 * the callable again.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto config = common::lazy([] {
 *   return load_config_from_disk();
 * });
 *
 * // load_config_from_disk() runs here, once.
 * auto const& first = config();
 * auto const& again = config();
 * @endcode
 *
 * The callable must be invocable without arguments and must return a storable value type.
 * @tparam F Nullary callable type.
 * @param f Callable used to produce the cached value.
 * @return Lazy value wrapper.
 * @ingroup core_functional
 * @see defer
 */
template <typename F>
[[nodiscard]] auto lazy(F&& f)
{
  return _lazy_impl::lazy<std::remove_cvref_t<F>>(std::forward<F>(f));
}

RADAR_COMMON_NAMESPACE_END
