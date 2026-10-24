#pragma once

#include <cstdint>

#include <filesystem>
#include <string_view>
#include <system_error>
#include <utility>

#include <whs31/common/error.h>
#include <whs31/common/detail/namespace.h>
#include <whs31/common/detail/export.h>

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Error raised when a platform path cannot be resolved.
 * @ingroup core_errors
 * @ingroup core_filesystem
 * @see Error
 */
class WHS31_COMMON_API PathError : public Error
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
[[nodiscard]] WHS31_COMMON_API std::filesystem::path user_home_path();

/**
 * @brief Returns the current user's home directory.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Home directory path, or an empty path on failure.
 */
[[nodiscard]] WHS31_COMMON_API std::filesystem::path user_home_path(std::error_code& ec) noexcept;

/**
 * @brief Returns the current executable file path.
 * @return Absolute path to the running executable.
 * @throws PathError If the executable path cannot be resolved.
 */
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_file_path();

/**
 * @brief Returns the current executable file path.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Absolute path to the running executable, or an empty path on failure.
 */
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_file_path(
  std::error_code& ec
) noexcept;

/**
 * @brief Returns the current executable directory path.
 * @return Directory containing the running executable.
 * @throws PathError If the executable path cannot be resolved.
 */
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_dir_path();

/**
 * @brief Returns the current executable directory path.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Directory containing the running executable, or an empty path on failure.
 */
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_dir_path(
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
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_data_dir_path(
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
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_data_dir_path(
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
 * using namespace whs31;
 *
 * auto cache = application_local_data_dir_path("Vendor", "product");
 * auto lock = LockFile(cache / "product.lock");
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
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_local_data_dir_path(
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
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_local_data_dir_path(
  std::string_view app_dir,
  std::string_view app_name,
  std::error_code& ec
) noexcept;

/**
 * @brief Category of the per-user files of an application.
 * @ingroup core_filesystem
 * @see application_directory_path()
 */
enum class ApplicationDirectory : std::uint8_t
{
  Root,     ///< Parent of the other categories where they share one; the data directory on Linux.
  Config,   ///< Configuration the user may edit, e.g. settings.
  Data,     ///< Durable user data, e.g. plugins or documents.
  State,    ///< Session state, history and logs.
  Cache,    ///< Data that can be deleted and computed or downloaded again.
  Runtime,  ///< Locks, sockets and other files of the running session.
};

/**
 * @brief Where application_directory_path() places the per-user files of an application.
 * @ingroup core_filesystem
 */
enum class ApplicationLayout : std::uint8_t
{
  Portable,  ///< Under `data/` next to the executable.
  Standard,  ///< The platform's per-user folders: AppData on Windows, XDG base directories on
             ///< Linux.
};

/**
 * @brief Returns the directory of one category of the per-user files of an application.
 * @details Every application of the organization lays its files out the same way, so that two
 * programs naming the same vendor and product, e.g. a command-line tool and a GUI application,
 * share them. In the standard layout:
 *
 * | Category | Linux                       | Windows                                   | Other |
 * |----------|-----------------------------|-------------------------------------------|--------------------------|
 * | Root     | `$XDG_DATA_HOME/<product>`  | `%APPDATA%/<vendor>/<product>`            |
 * `~/.local/<product>`     | | Config   | `$XDG_CONFIG_HOME/<product>`| `<root>/config` |
 * `<root>/config`          | | Data     | `$XDG_DATA_HOME/<product>`  | `<root>/data` |
 * `<root>/data`            | | State    | `$XDG_STATE_HOME/<product>` | `<root>/data` |
 * `<root>/data`            | | Cache    | `$XDG_CACHE_HOME/<product>` | `<root>/cache` |
 * `<root>/cache`           | | Runtime  | `$XDG_RUNTIME_DIR/<product>`|
 * `%LOCALAPPDATA%/<vendor>/<product>/runtime` | `<root>/runtime`       |
 *
 * Unset or relative XDG homes fall back to `~/.config`, `~/.local/share`, `~/.local/state` and
 * `~/.cache`. The runtime directory has no fallback: `$XDG_RUNTIME_DIR` must be set and absolute.
 *
 * The portable layout keeps everything under `<executable directory>/data`: `config`, `data`,
 * `cache` and `runtime` below it, the state in `data`.
 *
 * Nothing is created; whoever writes into a directory creates it.
 *
 * @code {.cpp}
 * using namespace whs31;
 *
 * auto const settings = application_directory_path(ApplicationDirectory::Config, "Vendor",
 * "product") / "settings.json";
 * @endcode
 *
 * @param directory Category to resolve.
 * @param vendor Vendor component, used on Windows.
 * @param product Application component, the last component of every standard directory.
 * @param layout Standard per-user folders or portable.
 * @return Absolute directory path.
 * @throws PathError If either name is not a single non-empty path component, or a base directory
 * cannot be resolved (no home directory, executable or `$XDG_RUNTIME_DIR`).
 * @ingroup core_filesystem
 */
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_directory_path(
  ApplicationDirectory directory,
  std::string_view vendor,
  std::string_view product,
  ApplicationLayout layout = ApplicationLayout::Standard
);

/**
 * @brief Returns the directory of one category of the per-user files of an application.
 * @param directory Category to resolve.
 * @param vendor Vendor component, used on Windows.
 * @param product Application component.
 * @param layout Standard per-user folders or portable.
 * @param ec Receives an error code on failure and is cleared on success.
 * @return Absolute directory path, or an empty path on failure.
 * @see application_directory_path(ApplicationDirectory, std::string_view, std::string_view,
 * ApplicationLayout)
 */
[[nodiscard]] WHS31_COMMON_API std::filesystem::path application_directory_path(
  ApplicationDirectory directory,
  std::string_view vendor,
  std::string_view product,
  ApplicationLayout layout,
  std::error_code& ec
) noexcept;

#ifdef _WIN32
namespace win32
{
  /**
   * @brief Returns the Windows roaming application data directory.
   * @return `%APPDATA%` path.
   */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path appdata_dir_path();

  /**
   * @brief Returns the Windows roaming application data directory.
   * @param ec Receives an error code on failure and is cleared on success.
   * @return `%APPDATA%` path, or an empty path on failure.
   */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path appdata_dir_path(
    std::error_code& ec
  ) noexcept;

  /**
   * @brief Returns the Windows local application data directory.
   * @return `%LOCALAPPDATA%` path.
   */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path local_appdata_dir_path();

  /**
   * @brief Returns the Windows local application data directory.
   * @param ec Receives an error code on failure and is cleared on success.
   * @return `%LOCALAPPDATA%` path, or an empty path on failure.
   */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path local_appdata_dir_path(
    std::error_code& ec
  ) noexcept;
}  // namespace win32
#endif

#ifdef __linux__
namespace nix_  // NOLINT(readability-identifier-naming) -- public name avoids macro collisions.
{
  /** @brief Returns the XDG configuration home, with the specification fallback. */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path xdg_config_home_path();

  /** @brief Returns the XDG data home, with the specification fallback. */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path xdg_data_home_path();

  /** @brief Returns the XDG state home, with the specification fallback. */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path xdg_state_home_path();

  /** @brief Returns the XDG cache home, with the specification fallback. */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path xdg_cache_home_path();

  /**
   * @brief Returns the XDG runtime directory.
   * @return Path stored in `$XDG_RUNTIME_DIR`.
   * @throws PathError If `$XDG_RUNTIME_DIR` is unset, empty, or relative.
   */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path xdg_runtime_dir_path();

  /**
   * @brief Returns the XDG runtime directory.
   * @param ec Receives an error code on failure and is cleared on success.
   * @return Path stored in `$XDG_RUNTIME_DIR`, or an empty path on failure.
   */
  [[nodiscard]] WHS31_COMMON_API std::filesystem::path xdg_runtime_dir_path(
    std::error_code& ec
  ) noexcept;
}  // namespace nix_
#endif

WHS31_COMMON_NAMESPACE_END
