#pragma once

#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <utility>

#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Scoped shared access to a value protected by a std::shared_mutex.
 * @details The guard owns a std::shared_lock for the supplied mutex and exposes const
 * pointer-like access to the protected object while the lock is held.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto mutex = std::shared_mutex();
 * auto cache = Cache();
 *
 * auto guard = RwLockReadGuard(mutex, cache);
 * return guard->lookup(key);
 * @endcode
 *
 * @tparam T Protected value type.
 * @ingroup core_threading
 * @see RwLockWriteGuard
 * @see MutexGuard
 */
template <typename T>
class [[nodiscard]] RwLockReadGuard
{
 public:
  RwLockReadGuard(std::shared_mutex& mutex, T const& ref)
    : m_lock(mutex)
    , m_ref(ref)
  {}

  /**
   * @throws std::invalid_argument if @p lock does not own its mutex.
   */
  RwLockReadGuard(std::shared_lock<std::shared_mutex>&& lock, T const& ref)
    : m_lock(std::move(lock))
    , m_ref(ref)
  {
    if(not this->m_lock.owns_lock())
      throw std::invalid_argument("RwLockReadGuard requires an owned lock");
  }

  RwLockReadGuard(RwLockReadGuard const&) = delete;
  RwLockReadGuard(RwLockReadGuard&&) = delete;
  RwLockReadGuard& operator=(RwLockReadGuard const&) = delete;
  RwLockReadGuard& operator=(RwLockReadGuard&&) = delete;
  ~RwLockReadGuard() = default;

  [[nodiscard]] T const* operator->() const noexcept { return &this->m_ref; }

  [[nodiscard]] T const& operator*() const noexcept { return this->m_ref; }

 private:
  std::shared_lock<std::shared_mutex> m_lock;
  T const& m_ref;
};

/**
 * @brief Scoped exclusive access to a value protected by a std::shared_mutex.
 * @details The guard owns a std::unique_lock for the supplied mutex and exposes mutable
 * pointer-like access to the protected object while the lock is held.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto mutex = std::shared_mutex();
 * auto cache = Cache();
 *
 * auto guard = RwLockWriteGuard(mutex, cache);
 * guard->insert(key, value);
 * @endcode
 *
 * @tparam T Protected value type.
 * @ingroup core_threading
 * @see RwLockReadGuard
 * @see MutexGuard
 */
template <typename T>
class [[nodiscard]] RwLockWriteGuard
{
 public:
  RwLockWriteGuard(std::shared_mutex& mutex, T& ref)
    : m_lock(mutex)
    , m_ref(ref)
  {}

  /**
   * @throws std::invalid_argument if @p lock does not own its mutex.
   */
  RwLockWriteGuard(std::unique_lock<std::shared_mutex>&& lock, T& ref)
    : m_lock(std::move(lock))
    , m_ref(ref)
  {
    if(not this->m_lock.owns_lock())
      throw std::invalid_argument("RwLockWriteGuard requires an owned lock");
  }

  RwLockWriteGuard(RwLockWriteGuard const&) = delete;
  RwLockWriteGuard(RwLockWriteGuard&&) = delete;
  RwLockWriteGuard& operator=(RwLockWriteGuard const&) = delete;
  RwLockWriteGuard& operator=(RwLockWriteGuard&&) = delete;
  ~RwLockWriteGuard() = default;

  [[nodiscard]] T* operator->() noexcept { return &this->m_ref; }

  [[nodiscard]] T const* operator->() const noexcept { return &this->m_ref; }

  [[nodiscard]] T& operator*() noexcept { return this->m_ref; }

  [[nodiscard]] T const& operator*() const noexcept { return this->m_ref; }

 private:
  std::unique_lock<std::shared_mutex> m_lock;
  T& m_ref;
};

RADAR_COMMON_NAMESPACE_END
