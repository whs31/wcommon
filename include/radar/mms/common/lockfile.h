#pragma once

#include <filesystem>
#include <string>
#include <utility>

#include <radar/mms/common/error.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/detail/export.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Base class for lock file errors.
 * @ingroup core_filesystem
 * @see Error
 */
class RADAR_COMMON_API LockFileError : public Error
{
 public:
  /**
   * @brief Constructs a lock file error with a formatted message.
   * @tparam Args Format argument types.
   * @param fmt Format string.
   * @param args Format arguments.
   */
  template <typename... Args>
  explicit LockFileError(fmt::format_string<Args...> fmt, Args&&... args)
    : Error(fmt, std::forward<Args>(args)...)
  {}

  [[nodiscard]] static LockFileError already_locked(std::filesystem::path const& path);
  [[nodiscard]] static LockFileError cannot_create(
    std::filesystem::path const& path,
    int error_code
  );
  [[nodiscard]] static LockFileError cannot_write(
    std::filesystem::path const& path,
    int error_code
  );
  [[nodiscard]] static LockFileError cannot_create_directory(
    std::filesystem::path const& path,
    std::error_code error_code
  );
};

/**
 * @brief RAII lock represented by an exclusive file on disk.
 * @details The lock is acquired by atomically creating the lock file. A locked
 * instance removes the lock file when `unlock()` is called or when the object is
 * destroyed. Each instance writes an ownership token and will not remove a file
 * that has since been replaced by another owner. The class is move-only because
 * it owns the on-disk lock.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto lock = LockFile("/tmp/app.lock");
 * if(not lock.try_lock())
 *   return already_running();
 *
 * run_single_instance_work();
 * @endcode
 *
 * @ingroup core_filesystem
 * @see paths.h
 * @see LockFileError
 */
class RADAR_COMMON_API LockFile
{
 public:
  /**
   * @brief Creates an unlocked lock object for a path.
   * @param path Lock file path.
   */
  explicit LockFile(std::filesystem::path const& path);

  /**
   * @brief Creates an unlocked lock object for a path.
   * @param path Lock file path.
   */
  explicit LockFile(std::filesystem::path&& path);

  LockFile(LockFile const&) = delete;
  LockFile& operator=(LockFile const&) = delete;
  LockFile(LockFile&& other) noexcept;
  LockFile& operator=(LockFile&& other) noexcept;

  /**
   * @brief Releases the lock if this object owns one.
   */
  ~LockFile();

  /**
   * @brief Returns the configured lock file path.
   */
  [[nodiscard]] std::filesystem::path const& path() const noexcept;

  /**
   * @brief Returns true if this object currently owns the lock.
   */
  [[nodiscard]] bool is_locked() const noexcept;

  /**
   * @brief Attempts to acquire the lock without throwing.
   * @return True if the lock was acquired, false otherwise.
   */
  [[nodiscard]] bool try_lock() noexcept;

  /**
   * @brief Acquires the lock.
   * @throws LockFileError If this object or another process already owns the lock.
   * @throws LockFileError If the lock cannot be created.
   */
  void lock();

  /**
   * @brief Releases the lock if this object owns it.
   */
  void unlock() noexcept;

 private:
  bool m_locked;
  std::filesystem::path m_path;
  std::string m_token;
};

RADAR_COMMON_NAMESPACE_END
