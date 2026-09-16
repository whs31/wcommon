#pragma once

#include <filesystem>
#include <string_view>
#include <system_error>
#include <utility>

#include <radar/mms/common/error.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/detail/export.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Error raised when a platform path cannot be resolved.
 * @ingroup core_errors
 * @ingroup core_filesystem
 * @see Error
 */
class RADAR_COMMON_API PathError : public Error
{
 public:
  /**
   * @brief Constructs a path error with a formatted message.
   * @tparam Args Format argument types.
   * @param fmt Format string.
   * @param args Format arguments.
   */
  template <typename... Args>
  explicit PathError(fmt::format_string<Args...> fmt, Args&&... args)
    : Error(fmt, std::forward<Args>(args)...)
  {}
};

/**
 * @brief Returns the current user's home directory.
 * @return Home directory path.
 * @throws PathError If the home directory cannot be resolved.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path user_home_path();

/**
 * @brief Returns the current user's home directory.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Home directory path, or an empty path on failure.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path user_home_path(std::error_code& ec) noexcept;

/**
 * @brief Returns the current executable file path.
 * @return Absolute path to the running executable.
 * @throws PathError If the executable path cannot be resolved.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path application_file_path();

/**
 * @brief Returns the current executable file path.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Absolute path to the running executable, or an empty path on failure.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path application_file_path(
  std::error_code& ec
) noexcept;

/**
 * @brief Returns the current executable directory path.
 * @return Directory containing the running executable.
 * @throws PathError If the executable path cannot be resolved.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path application_dir_path();

/**
 * @brief Returns the current executable directory path.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Directory containing the running executable, or an empty path on failure.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path application_dir_path(
  std::error_code& ec
) noexcept;

/**
 * @brief Returns a roaming application data directory.
 * @details On Windows this is `%APPDATA% / app_dir / app_name`. On Linux it is
 * `$XDG_DATA_HOME / app_name`, using the XDG fallback when unset. Other Unix-like platforms return
 * `$HOME / ".local" / app_name`.
 * @param app_dir Vendor or application directory component used on Windows.
 * @param app_name Application name component.
 * @return Application data directory path.
 * @throws PathError If either name is not a single non-empty path component.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path application_data_dir_path(
  std::string_view app_dir,
  std::string_view app_name
);

/**
 * @brief Returns a roaming application data directory.
 * @param app_dir Vendor or application directory component used on Windows.
 * @param app_name Application name component.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Application data directory path, or an empty path on failure.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path application_data_dir_path(
  std::string_view app_dir,
  std::string_view app_name,
  std::error_code& ec
) noexcept;

/**
 * @brief Returns a local application data directory.
 * @details On Windows this is `%LOCALAPPDATA% / app_dir / app_name`. On Linux it is
 * `$XDG_DATA_HOME / app_name`, using the XDG fallback when unset. Other Unix-like platforms return
 * `$HOME / ".local" / app_name`.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto cache = application_local_data_dir_path("MMS", "Radar");
 * auto lock = LockFile(cache / "radar.lock");
 * lock.lock();
 * @endcode
 *
 * @param app_dir Vendor or application directory component used on Windows.
 * @param app_name Application name component.
 * @return Local application data directory path.
 * @throws PathError If either name is not a single non-empty path component.
 * @ingroup core_filesystem
 * @see LockFile
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path application_local_data_dir_path(
  std::string_view app_dir,
  std::string_view app_name
);

/**
 * @brief Returns a local application data directory.
 * @param app_dir Vendor or application directory component used on Windows.
 * @param app_name Application name component.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Local application data directory path, or an empty path on failure.
 */
[[nodiscard]] RADAR_COMMON_API std::filesystem::path application_local_data_dir_path(
  std::string_view app_dir,
  std::string_view app_name,
  std::error_code& ec
) noexcept;

#ifdef _WIN32
namespace win32
{
  /**
   * @brief Returns the Windows roaming application data directory.
   * @return `%APPDATA%` path.
   */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path appdata_dir_path();

  /**
   * @brief Returns the Windows roaming application data directory.
   * @param ec Receives an error code on failure and is cleared on success.
   * @return `%APPDATA%` path, or an empty path on failure.
   */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path appdata_dir_path(
    std::error_code& ec
  ) noexcept;

  /**
   * @brief Returns the Windows local application data directory.
   * @return `%LOCALAPPDATA%` path.
   */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path local_appdata_dir_path();

  /**
   * @brief Returns the Windows local application data directory.
   * @param ec Receives an error code on failure and is cleared on success.
   * @return `%LOCALAPPDATA%` path, or an empty path on failure.
   */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path local_appdata_dir_path(
    std::error_code& ec
  ) noexcept;
}  // namespace win32
#endif

#ifdef __linux__
namespace nix_  // NOLINT(readability-identifier-naming) -- public name avoids macro collisions.
{
  /** @brief Returns the XDG configuration home, with the specification fallback. */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path xdg_config_home_path();

  /** @brief Returns the XDG data home, with the specification fallback. */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path xdg_data_home_path();

  /** @brief Returns the XDG state home, with the specification fallback. */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path xdg_state_home_path();

  /** @brief Returns the XDG cache home, with the specification fallback. */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path xdg_cache_home_path();

  /**
   * @brief Returns the XDG runtime directory.
   * @return Path stored in `$XDG_RUNTIME_DIR`.
   * @throws PathError If `$XDG_RUNTIME_DIR` is unset, empty, or relative.
   */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path xdg_runtime_dir_path();

  /**
   * @brief Returns the XDG runtime directory.
   * @param ec Receives an error code on failure and is cleared on success.
   * @return Path stored in `$XDG_RUNTIME_DIR`, or an empty path on failure.
   */
  [[nodiscard]] RADAR_COMMON_API std::filesystem::path xdg_runtime_dir_path(
    std::error_code& ec
  ) noexcept;
}  // namespace nix_
#endif

RADAR_COMMON_NAMESPACE_END
