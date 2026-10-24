#include <whs31/common/lockfile.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>
#include <type_traits>

#ifdef _WIN32
#  include <windows.h>
#endif

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

namespace
{
  [[nodiscard]] std::filesystem::path lockfile_test_path(char const* const name)
  {
    auto const path = std::filesystem::temp_directory_path() / "wcommon_lockfile_tests";
    std::filesystem::create_directories(path);
    return path / name;
  }

  [[nodiscard]] std::string read_file(std::filesystem::path const& path)
  {
    auto input = std::ifstream(path, std::ios::binary);
    return {std::istreambuf_iterator<char> {input}, std::istreambuf_iterator<char> {}};
  }
}  // namespace

TEST_CASE("LockFile is move-only", "[lockfile]")
{
  using CommonLockFile = whs31::common::LockFile;

  STATIC_CHECK(not std::is_copy_constructible_v<CommonLockFile>);
  STATIC_CHECK(not std::is_copy_assignable_v<CommonLockFile>);
  STATIC_CHECK(std::is_nothrow_move_constructible_v<CommonLockFile>);
  STATIC_CHECK(std::is_nothrow_move_assignable_v<CommonLockFile>);
}

TEST_CASE("LockFile creates and removes an owned lock marker", "[lockfile]")
{
  auto const path = lockfile_test_path("basic.lock");
  std::filesystem::remove(path);

  {
    auto lock = whs31::common::LockFile(path);
    REQUIRE(lock.path() == path);
    REQUIRE_FALSE(lock.is_locked());

    lock.lock();
    REQUIRE(lock.is_locked());
    REQUIRE(std::filesystem::exists(path));
    REQUIRE(read_file(path).starts_with("locked:"));

    lock.unlock();
    REQUIRE_FALSE(lock.is_locked());
    REQUIRE_FALSE(std::filesystem::exists(path));
  }

  REQUIRE_FALSE(std::filesystem::exists(path));
}

TEST_CASE("LockFile does not remove a replacement lock owned by another instance", "[lockfile]")
{
  auto const path = lockfile_test_path("replaced.lock");
  std::filesystem::remove(path);

  auto first = whs31::common::LockFile(path);
  auto second = whs31::common::LockFile(path);
  REQUIRE(first.try_lock());

  std::filesystem::remove(path);
  REQUIRE(second.try_lock());
  auto const second_marker = read_file(path);

  first.unlock();
  REQUIRE_FALSE(first.is_locked());
  REQUIRE(std::filesystem::exists(path));
  REQUIRE(read_file(path) == second_marker);

  second.unlock();
}

TEST_CASE("LockFile refuses to acquire an existing lock", "[lockfile]")
{
  auto const path = lockfile_test_path("existing.lock");
  std::filesystem::remove(path);

  auto first = whs31::common::LockFile(path);
  auto second = whs31::common::LockFile(path);

  REQUIRE(first.try_lock());
  REQUIRE(first.is_locked());
  REQUIRE_FALSE(second.try_lock());
  REQUIRE_THROWS_AS(second.lock(), LockFileError);

  first.unlock();
  REQUIRE(second.try_lock());
}

TEST_CASE("LockFile transfers ownership when moved", "[lockfile]")
{
  auto const path = lockfile_test_path("move.lock");
  auto const other_path = lockfile_test_path("move-other.lock");
  std::filesystem::remove(path);
  std::filesystem::remove(other_path);

  auto lock = whs31::common::LockFile(path);
  REQUIRE(lock.try_lock());

  auto moved = whs31::common::LockFile(std::move(lock));
  REQUIRE_FALSE(lock.is_locked());  // NOLINT(bugprone-use-after-move): checks the moved-from state
  REQUIRE(moved.is_locked());
  REQUIRE(std::filesystem::exists(path));

  moved = whs31::common::LockFile(other_path);
  REQUIRE_FALSE(std::filesystem::exists(path));
  REQUIRE_FALSE(moved.is_locked());
}

TEST_CASE("LockFile reports directory creation failures with project errors", "[lockfile]")
{
  auto const parent_file = lockfile_test_path("not-a-directory");
  auto const path = parent_file / "child.lock";
  std::filesystem::remove_all(parent_file);
  {
    auto marker = std::ofstream(parent_file);
    marker << "file";
  }

  auto lock = whs31::common::LockFile(path);
  REQUIRE_THROWS_AS(lock.lock(), LockFileError);
  REQUIRE_FALSE(lock.try_lock());

  std::filesystem::remove(parent_file);
}

#ifdef _WIN32
TEST_CASE("LockFile formats native Windows errors", "[lockfile]")
{
  auto const code = static_cast<int>(ERROR_ACCESS_DENIED);
  auto const expected = std::error_code(code, std::system_category()).message();
  auto const error = LockFileError::cannot_create("denied.lock", code);

  REQUIRE(std::string {error.what()}.contains(expected));
}
#endif
