#include <radar/mms/common/threading.h>

#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <string>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

namespace
{
  struct ProtectedValue
  {
    int value = 0;
    std::string name;
  };
}  // namespace

TEST_CASE("MutexGuard exposes mutable access while owning the lock", "[threading]")
{
  auto mutex = std::mutex();
  auto value = ProtectedValue {.value = 1, .name = "initial"};

  {
    auto guard = MutexGuard(mutex, value);
    guard->value = 2;
    (*guard).name = "updated";
  }

  REQUIRE(value.value == 2);
  REQUIRE(value.name == "updated");

  auto const lock = std::unique_lock {mutex, std::try_to_lock};
  REQUIRE(lock.owns_lock());
}

TEST_CASE("MutexGuard accepts an existing owned lock", "[threading]")
{
  auto mutex = std::mutex();
  auto value = ProtectedValue {.value = 1, .name = "initial"};

  {
    auto lock = std::unique_lock {mutex};
    auto guard = MutexGuard(std::move(lock), value);
    REQUIRE_FALSE(lock.owns_lock());  // NOLINT(bugprone-use-after-move): checks the moved-from lock
    guard->value = 3;
  }

  REQUIRE(value.value == 3);
}

TEST_CASE("MutexGuard rejects an unlocked lock", "[threading]")
{
  auto mutex = std::mutex();
  auto value = ProtectedValue {};
  auto lock = std::unique_lock {mutex, std::defer_lock};

  REQUIRE_THROWS_AS(MutexGuard(std::move(lock), value), std::invalid_argument);
}

TEST_CASE("RwLock guards expose shared read and exclusive write access", "[threading]")
{
  auto mutex = std::shared_mutex();
  auto value = ProtectedValue {.value = 1, .name = "initial"};

  {
    auto write_guard = RwLockWriteGuard(mutex, value);
    write_guard->value = 2;
    (*write_guard).name = "written";
  }

  {
    auto const read_guard = RwLockReadGuard(mutex, value);
    REQUIRE(read_guard->value == 2);
    REQUIRE((*read_guard).name == "written");
  }
}

TEST_CASE("RwLock guards accept existing owned locks", "[threading]")
{
  auto mutex = std::shared_mutex();
  auto value = ProtectedValue {.value = 1, .name = "initial"};

  {
    auto lock = std::unique_lock<std::shared_mutex> {mutex};
    auto write_guard = RwLockWriteGuard(std::move(lock), value);
    REQUIRE_FALSE(lock.owns_lock());  // NOLINT(bugprone-use-after-move): checks the moved-from lock
    write_guard->value = 4;
  }

  {
    auto lock = std::shared_lock {mutex};
    auto read_guard = RwLockReadGuard(std::move(lock), value);
    REQUIRE_FALSE(lock.owns_lock());  // NOLINT(bugprone-use-after-move): checks the moved-from lock
    REQUIRE(read_guard->value == 4);
  }
}

TEST_CASE("RwLock guards reject unlocked locks", "[threading]")
{
  auto mutex = std::shared_mutex();
  auto value = ProtectedValue {};

  auto write_lock = std::unique_lock<std::shared_mutex> {mutex, std::defer_lock};
  REQUIRE_THROWS_AS(RwLockWriteGuard(std::move(write_lock), value), std::invalid_argument);

  auto read_lock = std::shared_lock {mutex, std::defer_lock};
  REQUIRE_THROWS_AS(RwLockReadGuard(std::move(read_lock), value), std::invalid_argument);
}

TEST_CASE("Yield helpers are callable", "[threading]")
{
  REQUIRE_NOTHROW(yield());
  REQUIRE_NOTHROW(yield(0));
  REQUIRE_NOTHROW(yield(1));
}
