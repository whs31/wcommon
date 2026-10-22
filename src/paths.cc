#include <radar/mms/common/paths.h>

#include <array>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#ifdef _WIN32
#  if defined(_MSC_VER)
#    if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) \
      || defined(_M_X64) || defined(_M_AMD64)
#      define _AMD64_
#    elif defined(i386) || defined(__i386) || defined(__i386__) || defined(__i386__) \
      || defined(_M_IX86)
#      define _X86_
#    elif defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)
#      define _ARM_
#    endif
#  endif
#  include <shlobj.h>
#  include <windows.h>
#else
#  include <unistd.h>
#endif

namespace
{
  template <typename Fn>
  [[nodiscard]] std::filesystem::path checked_path(Fn&& fn, std::error_code& ec) noexcept
  {
    try {
      auto path = fn();
      ec.clear();
      return path;
    } catch(radar::mms::common::PathError const&) {
      ec = std::make_error_code(std::errc::io_error);
      return {};
    } catch(std::system_error const& e) {
      ec = e.code();
      return {};
    } catch(std::exception const&) {
      ec = std::make_error_code(std::errc::io_error);
      return {};
    } catch(...) {
      ec = std::make_error_code(std::errc::io_error);
      return {};
    }
  }

  void validate_path_component(std::string_view const value, char const* const name)
  {
    auto const path = std::filesystem::path(value);
    if(
      value.empty() or value == "." or value == ".." or value.contains('/') or value.contains('\\')
      or value.contains('\0') or path.has_root_path() or path.has_parent_path()
    )
      throw radar::mms::common::PathError("{} must be a single non-empty path component", name);
  }

#ifdef _WIN32
  [[nodiscard]] std::filesystem::path known_folder_path(::KNOWNFOLDERID const& id)
  {
    auto* buffer = ::PWSTR();
    auto const result = ::SHGetKnownFolderPath(id, 0, nullptr, &buffer);
    if(FAILED(result)) {
      ::CoTaskMemFree(buffer);
      throw radar::mms::common::PathError(
        "failed to get known folder path: {}",
        std::system_error(result, std::system_category()).what()
      );
    }

    auto path = std::filesystem::path(buffer);
    ::CoTaskMemFree(buffer);
    return path;
  }

  [[nodiscard]] radar::mms::common::PathError last_path_error(
    char const* operation,
    std::errc const fallback
  )
  {
    auto const error = ::GetLastError();
    if(error != 0)
      return radar::mms::common::PathError(
        "failed to {}: {}",
        operation,
        std::system_error(static_cast<int>(error), std::system_category()).what()
      );
    return radar::mms::common::
      PathError("failed to {}: {}", operation, std::make_error_code(fallback).message());
  }
#endif

  // the categories under one root: the portable layout, and the standard one outside linux
  [[nodiscard]] std::filesystem::path nested_directory_path(
    std::filesystem::path const& root,
    radar::mms::common::ApplicationDirectory const directory
  )
  {
    using radar::mms::common::ApplicationDirectory;
    switch(directory) {
      case ApplicationDirectory::Root: return root;
      case ApplicationDirectory::Config: return root / "config";
      case ApplicationDirectory::Data:
      case ApplicationDirectory::State: return root / "data";
      case ApplicationDirectory::Cache: return root / "cache";
      case ApplicationDirectory::Runtime: return root / "runtime";
    }
    throw radar::mms::common::
      PathError("unknown application directory {}", std::to_underlying(directory));
  }

#ifdef __linux__
  [[nodiscard]] std::filesystem::path xdg_home_path(
    char const* const variable,
    std::filesystem::path const& fallback
  )
  {
    if(auto const* value = std::getenv(variable); value != nullptr and *value != '\0') {
      if(auto path = std::filesystem::path(value); path.is_absolute())
        return path;
    }
    auto const home = radar::mms::common::user_home_path();
    if(not home.is_absolute())
      throw radar::mms::common::PathError(
        "failed to get XDG base directory: $HOME is not absolute"
      );
    return home / fallback;
  }
#endif
}  // namespace

RADAR_COMMON_NAMESPACE_BEGIN

std::filesystem::path user_home_path(std::error_code& ec) noexcept
{
  return checked_path([] { return user_home_path(); }, ec);
}

std::filesystem::path application_file_path(std::error_code& ec) noexcept
{
  return checked_path([] { return application_file_path(); }, ec);
}

std::filesystem::path application_dir_path(std::error_code& ec) noexcept
{
  return checked_path([] { return application_dir_path(); }, ec);
}

std::filesystem::path application_data_dir_path(
  std::string_view const app_dir,
  std::string_view const app_name,
  std::error_code& ec
) noexcept
{
  return checked_path([&] { return application_data_dir_path(app_dir, app_name); }, ec);
}

std::filesystem::path application_local_data_dir_path(
  std::string_view const app_dir,
  std::string_view const app_name,
  std::error_code& ec
) noexcept
{
  return checked_path([&] { return application_local_data_dir_path(app_dir, app_name); }, ec);
}

std::filesystem::path application_directory_path(
  ApplicationDirectory const directory,
  std::string_view const vendor,
  std::string_view const product,
  ApplicationLayout const layout,
  std::error_code& ec
) noexcept
{
  return checked_path(
    [&] { return application_directory_path(directory, vendor, product, layout); },
    ec
  );
}

#ifdef _WIN32
namespace win32
{
  std::filesystem::path appdata_dir_path(std::error_code& ec) noexcept
  {
    return checked_path([] { return appdata_dir_path(); }, ec);
  }

  std::filesystem::path local_appdata_dir_path(std::error_code& ec) noexcept
  {
    return checked_path([] { return local_appdata_dir_path(); }, ec);
  }

  std::filesystem::path appdata_dir_path() { return known_folder_path(FOLDERID_RoamingAppData); }

  std::filesystem::path local_appdata_dir_path()
  {
    return known_folder_path(FOLDERID_LocalAppData);
  }
}  // namespace win32
#endif

#ifdef __linux__
namespace nix_
{
  std::filesystem::path xdg_config_home_path()
  {
    return xdg_home_path("XDG_CONFIG_HOME", ".config");
  }

  std::filesystem::path xdg_data_home_path()
  {
    return xdg_home_path("XDG_DATA_HOME", ".local/share");
  }

  std::filesystem::path xdg_state_home_path()
  {
    return xdg_home_path("XDG_STATE_HOME", ".local/state");
  }

  std::filesystem::path xdg_cache_home_path() { return xdg_home_path("XDG_CACHE_HOME", ".cache"); }

  std::filesystem::path xdg_runtime_dir_path(std::error_code& ec) noexcept
  {
    return checked_path([] { return xdg_runtime_dir_path(); }, ec);
  }

  std::filesystem::path xdg_runtime_dir_path()
  {
    auto const* runtime_dir = std::getenv("XDG_RUNTIME_DIR");
    if(runtime_dir == nullptr or *runtime_dir == '\0')
      throw PathError("failed to get XDG_RUNTIME_DIR: $XDG_RUNTIME_DIR is not set");
    auto path = std::filesystem::path(runtime_dir);
    if(not path.is_absolute())
      throw PathError("failed to get XDG_RUNTIME_DIR: $XDG_RUNTIME_DIR is not absolute");
    return path;
  }
}  // namespace nix_
#endif

std::filesystem::path user_home_path()
{
#ifdef _WIN32
  auto buffer = std::array<wchar_t, MAX_PATH> {};
  auto const
    result = ::SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, SHGFP_TYPE_CURRENT, buffer.data());
  if(FAILED(result))
    throw PathError(
      "failed to get home directory: {}",
      std::system_error(result, std::system_category()).what()
    );
  return {buffer.data()};
#else
  auto const* home = std::getenv("HOME");
  if(home == nullptr or *home == '\0')
    throw PathError("failed to get home directory: $HOME is not set or empty");
  return {home};
#endif
}

std::filesystem::path application_file_path()
{
#ifdef _WIN32
  auto buffer = std::vector<wchar_t>(MAX_PATH);
  while(true) {
    auto const
      size = ::GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if(size == 0)
      throw last_path_error("get application file path", std::errc::no_such_file_or_directory);
    if(size < buffer.size() - 1)
      return {buffer.data()};
    buffer.resize(buffer.size() * 2);
  }
#else
  auto buffer = std::vector<char>(1'024);
  while(true) {
    auto const len = ::readlink("/proc/self/exe", buffer.data(), buffer.size());
    if(len == -1)
      throw PathError(
        "failed to get application file path: {}",
        std::system_error(errno, std::generic_category()).what()
      );
    if(static_cast<std::size_t>(len) < buffer.size())
      return {std::string {buffer.data(), static_cast<std::size_t>(len)}};
    buffer.resize(buffer.size() * 2);
  }
#endif
}

std::filesystem::path application_dir_path() { return application_file_path().parent_path(); }

std::filesystem::path application_data_dir_path(
  [[maybe_unused]] std::string_view const app_dir,
  std::string_view const app_name
)
{
  validate_path_component(app_dir, "app_dir");
  validate_path_component(app_name, "app_name");
#ifdef _WIN32
  return win32::appdata_dir_path() / app_dir / app_name;
#elifdef __linux__
  return nix_::xdg_data_home_path() / app_name;
#else
  return user_home_path() / ".local" / app_name;
#endif
}

std::filesystem::path application_local_data_dir_path(
  [[maybe_unused]] std::string_view const app_dir,
  std::string_view const app_name
)
{
  validate_path_component(app_dir, "app_dir");
  validate_path_component(app_name, "app_name");
#ifdef _WIN32
  return win32::local_appdata_dir_path() / app_dir / app_name;
#elifdef __linux__
  return nix_::xdg_data_home_path() / app_name;
#else
  return user_home_path() / ".local" / app_name;
#endif
}

// note: [whs31 : 2026-10-22]
// the layout of ApplicationPaths of mms 3: applications without mms (helisim) share its directories
std::filesystem::path application_directory_path(
  ApplicationDirectory const directory,
  std::string_view const vendor,
  std::string_view const product,
  ApplicationLayout const layout
)
{
  validate_path_component(vendor, "vendor");
  validate_path_component(product, "product");
  if(layout == ApplicationLayout::Portable)
    return nested_directory_path(application_dir_path() / "data", directory);
#ifdef __linux__
  switch(directory) {
    case ApplicationDirectory::Root:
    case ApplicationDirectory::Data: return nix_::xdg_data_home_path() / product;
    case ApplicationDirectory::Config: return nix_::xdg_config_home_path() / product;
    case ApplicationDirectory::State: return nix_::xdg_state_home_path() / product;
    case ApplicationDirectory::Cache: return nix_::xdg_cache_home_path() / product;
    case ApplicationDirectory::Runtime: return nix_::xdg_runtime_dir_path() / product;
  }
  throw PathError("unknown application directory {}", std::to_underlying(directory));
#else
  if(directory == ApplicationDirectory::Runtime) {
#  ifdef _WIN32
    return application_local_data_dir_path(vendor, product) / "runtime";
#  else
    return application_data_dir_path(vendor, product) / "runtime";
#  endif
  }
  return nested_directory_path(application_data_dir_path(vendor, product), directory);
#endif
}

RADAR_COMMON_NAMESPACE_END
