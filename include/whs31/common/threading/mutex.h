#pragma once

#include <mutex>
#include <stdexcept>
#include <utility>

#include <whs31/common/detail/namespace.h>

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Scoped mutable access to a value protected by a std::mutex.
 * @details The guard owns a std::unique_lock for the supplied mutex and exposes pointer-like
 * access to the protected object while the lock is held.
 *
 * @code {.cpp}
 * using namespace whs31;
 *
 * auto mutex = std::mutex();
 * auto state = State();
 *
 * {
 *   auto guard = MutexGuard(mutex, state);
 *   guard->counter += 1;
 * }
 * @endcode
 *
 * @tparam T Protected value type.
 * @ingroup core_threading
 * @see RwLockReadGuard
 * @see RwLockWriteGuard
 */
template <typename T>
class [[nodiscard]] MutexGuard
{
 public:
  MutexGuard(std::mutex& mutex, T& ref)
    : m_lock(mutex)
    , m_ref(ref)
  {}

  /**
   * @throws std::invalid_argument if @p lock does not own its mutex.
   */
  MutexGuard(std::unique_lock<std::mutex>&& lock, T& ref)
    : m_lock(std::move(lock))
    , m_ref(ref)
  {
    if(not this->m_lock.owns_lock())
      throw std::invalid_argument("MutexGuard requires an owned lock");
  }

  MutexGuard(MutexGuard const&) = delete;
  MutexGuard(MutexGuard&&) = delete;
  MutexGuard& operator=(MutexGuard const&) = delete;
  MutexGuard& operator=(MutexGuard&&) = delete;
  ~MutexGuard() = default;

  [[nodiscard]] T* operator->() noexcept { return &this->m_ref; }

  [[nodiscard]] T const* operator->() const noexcept { return &this->m_ref; }

  [[nodiscard]] T& operator*() noexcept { return this->m_ref; }

  [[nodiscard]] T const& operator*() const noexcept { return this->m_ref; }

 private:
  std::unique_lock<std::mutex> m_lock;
  T& m_ref;
};

WHS31_COMMON_NAMESPACE_END
