#include <whs31/common/paths.h>

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include <system_error>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

#ifdef __linux__
namespace
{
  class ScopedEnvironment
  {
   public:
    ScopedEnvironment(char const* const name, std::optional<std::string> value)
      : m_name(name)
    {
      if(auto const* previous = std::getenv(name); previous != nullptr)
        this->m_previous = previous;
      if(value)
        REQUIRE(::setenv(name, value->c_str(), 1) == 0);
      else
        REQUIRE(::unsetenv(name) == 0);
    }

    ~ScopedEnvironment()
    {
      if(this->m_previous)
        static_cast<void>(::setenv(this->m_name.c_str(), this->m_previous->c_str(), 1));
      else
        static_cast<void>(::unsetenv(this->m_name.c_str()));
    }

    ScopedEnvironment(ScopedEnvironment const&) = delete;
    ScopedEnvironment& operator=(ScopedEnvironment const&) = delete;
    ScopedEnvironment(ScopedEnvironment&&) = delete;
    ScopedEnvironment& operator=(ScopedEnvironment&&) = delete;

   private:
    std::string m_name;
    std::optional<std::string> m_previous;
  };
}  // namespace
#endif

TEST_CASE("Paths resolves the current user home directory", "[paths]")
{
  auto ec = std::error_code();
  auto const home = user_home_path(ec);

  REQUIRE_FALSE(ec);
  REQUIRE_FALSE(home.empty());

  if(auto const* env_home = std::getenv("HOME"); env_home != nullptr)
    REQUIRE(home == std::filesystem::path {env_home});
}

TEST_CASE("Paths resolves the current executable and directory", "[paths]")
{
  auto file_ec = std::error_code();
  auto dir_ec = std::error_code();
  auto const file = application_file_path(file_ec);
  auto const dir = application_dir_path(dir_ec);

  REQUIRE_FALSE(file_ec);
  REQUIRE_FALSE(dir_ec);
  REQUIRE_FALSE(file.empty());
  REQUIRE_FALSE(dir.empty());
  REQUIRE(file.parent_path() == dir);
  REQUIRE(std::filesystem::exists(file));
  REQUIRE(std::filesystem::is_directory(dir));
}

TEST_CASE("Paths creates application data paths from app names", "[paths]")
{
  auto data_ec = std::error_code();
  auto local_ec = std::error_code();
  auto const data = application_data_dir_path("WCommon", "Core Paths Test", data_ec);
  auto const local = application_local_data_dir_path("WCommon", "Core Paths Test", local_ec);

  REQUIRE_FALSE(data_ec);
  REQUIRE_FALSE(local_ec);
  REQUIRE_FALSE(data.empty());
  REQUIRE_FALSE(local.empty());
  REQUIRE(data.filename() == "Core Paths Test");
  REQUIRE(local.filename() == "Core Paths Test");

#ifdef __linux__
  REQUIRE(data == nix_::xdg_data_home_path() / "Core Paths Test");
  REQUIRE(local == data);
#elifndef _WIN32
  REQUIRE(data == user_home_path() / ".local" / "Core Paths Test");
  REQUIRE(local == data);
#endif
}

TEST_CASE("Paths rejects application data path traversal", "[paths]")
{
  for(auto const invalid : {
        std::string_view {},
        std::string_view {"."},
        std::string_view {".."},
        std::string_view {"../escape"},
        std::string_view {"nested/name"},
        std::string_view {"nested\\name"},
        std::string_view {"/absolute"},
      }) {
    CAPTURE(invalid);
    auto ec = std::error_code();
    CHECK(application_data_dir_path("WCommon", invalid, ec).empty());
    CHECK(ec);
    CHECK_THROWS_AS(application_data_dir_path("WCommon", invalid), PathError);
  }

  CHECK_THROWS_AS(application_data_dir_path("../vendor", "app"), PathError);
  CHECK_THROWS_AS(application_local_data_dir_path("vendor/name", "app"), PathError);
}

#ifdef __linux__
TEST_CASE("Paths rejects an empty user home", "[paths]")
{
  auto home = ScopedEnvironment("HOME", "");
  auto ec = std::error_code();

  REQUIRE(user_home_path(ec).empty());
  REQUIRE(ec);
  REQUIRE_THROWS_AS(user_home_path(), PathError);
}

TEST_CASE("Paths resolves absolute XDG overrides without HOME", "[paths]")
{
  auto const base = std::filesystem::temp_directory_path() / "wcommon-xdg-overrides";
  auto home = ScopedEnvironment("HOME", std::nullopt);
  auto config = ScopedEnvironment("XDG_CONFIG_HOME", (base / "config").string());
  auto data = ScopedEnvironment("XDG_DATA_HOME", (base / "data").string());
  auto state = ScopedEnvironment("XDG_STATE_HOME", (base / "state").string());
  auto cache = ScopedEnvironment("XDG_CACHE_HOME", (base / "cache").string());

  CHECK(nix_::xdg_config_home_path() == base / "config");
  CHECK(nix_::xdg_data_home_path() == base / "data");
  CHECK(nix_::xdg_state_home_path() == base / "state");
  CHECK(nix_::xdg_cache_home_path() == base / "cache");
}

TEST_CASE("Paths rejects relative XDG homes and uses defaults", "[paths]")
{
  auto const home_path = std::filesystem::temp_directory_path() / "wcommon-xdg-home";
  auto home = ScopedEnvironment("HOME", home_path.string());
  auto config = ScopedEnvironment("XDG_CONFIG_HOME", "relative/config");
  auto data = ScopedEnvironment("XDG_DATA_HOME", "");
  auto state = ScopedEnvironment("XDG_STATE_HOME", "relative/state");
  auto cache = ScopedEnvironment("XDG_CACHE_HOME", std::nullopt);

  CHECK(nix_::xdg_config_home_path() == home_path / ".config");
  CHECK(nix_::xdg_data_home_path() == home_path / ".local/share");
  CHECK(nix_::xdg_state_home_path() == home_path / ".local/state");
  CHECK(nix_::xdg_cache_home_path() == home_path / ".cache");
}

TEST_CASE("Paths rejects a relative HOME for XDG defaults", "[paths]")
{
  auto home = ScopedEnvironment("HOME", "relative/home");
  auto config = ScopedEnvironment("XDG_CONFIG_HOME", std::nullopt);

  CHECK_THROWS_AS(nix_::xdg_config_home_path(), PathError);
}

TEST_CASE("Paths requires an absolute XDG runtime directory", "[paths]")
{
  SECTION("missing")
  {
    auto runtime = ScopedEnvironment("XDG_RUNTIME_DIR", std::nullopt);
    auto ec = std::error_code();
    CHECK(nix_::xdg_runtime_dir_path(ec).empty());
    CHECK(ec);
    CHECK_THROWS_AS(nix_::xdg_runtime_dir_path(), PathError);
  }

  SECTION("relative")
  {
    auto runtime = ScopedEnvironment("XDG_RUNTIME_DIR", "relative/runtime");
    auto ec = std::error_code();
    CHECK(nix_::xdg_runtime_dir_path(ec).empty());
    CHECK(ec);
    CHECK_THROWS_AS(nix_::xdg_runtime_dir_path(), PathError);
  }

  SECTION("absolute")
  {
    auto const path = std::filesystem::temp_directory_path() / "wcommon-runtime";
    auto runtime = ScopedEnvironment("XDG_RUNTIME_DIR", path.string());
    auto ec = std::error_code();
    CHECK(nix_::xdg_runtime_dir_path(ec) == path);
    CHECK_FALSE(ec);
  }
}
#endif

TEST_CASE("Paths lays out the portable application directories", "[paths]")
{
  auto const root = application_dir_path() / "data";
  auto const path = [](ApplicationDirectory const directory) {
    return application_directory_path(
      directory,
      "WCommon",
      "Core Paths Test",
      ApplicationLayout::Portable
    );
  };

  CHECK(path(ApplicationDirectory::Root) == root);
  CHECK(path(ApplicationDirectory::Config) == root / "config");
  CHECK(path(ApplicationDirectory::Data) == root / "data");
  CHECK(path(ApplicationDirectory::State) == root / "data");
  CHECK(path(ApplicationDirectory::Cache) == root / "cache");
  CHECK(path(ApplicationDirectory::Runtime) == root / "runtime");
}

TEST_CASE("Paths rejects application directory names of several components", "[paths]")
{
  CHECK_THROWS_AS(
    application_directory_path(ApplicationDirectory::Config, "../vendor", "app"),
    PathError
  );
  CHECK_THROWS_AS(
    application_directory_path(
      ApplicationDirectory::Config,
      "vendor",
      "nested/app",
      ApplicationLayout::Portable
    ),
    PathError
  );

  auto ec = std::error_code();
  CHECK(application_directory_path(
          ApplicationDirectory::Data,
          "vendor",
          "",
          ApplicationLayout::Standard,
          ec
  )
          .empty());
  CHECK(ec);
}

#ifdef __linux__
TEST_CASE(
  "Paths places the standard application directories in the XDG base directories",
  "[paths]"
)
{
  auto const base = std::filesystem::temp_directory_path() / "wcommon-application-directories";
  auto config = ScopedEnvironment("XDG_CONFIG_HOME", (base / "config").string());
  auto data = ScopedEnvironment("XDG_DATA_HOME", (base / "data").string());
  auto state = ScopedEnvironment("XDG_STATE_HOME", (base / "state").string());
  auto cache = ScopedEnvironment("XDG_CACHE_HOME", (base / "cache").string());
  auto runtime = ScopedEnvironment("XDG_RUNTIME_DIR", (base / "runtime").string());
  auto const path = [](ApplicationDirectory const directory) {
    return application_directory_path(directory, "WCommon", "product");
  };

  CHECK(path(ApplicationDirectory::Root) == base / "data" / "product");
  CHECK(path(ApplicationDirectory::Config) == base / "config" / "product");
  CHECK(path(ApplicationDirectory::Data) == base / "data" / "product");
  CHECK(path(ApplicationDirectory::State) == base / "state" / "product");
  CHECK(path(ApplicationDirectory::Cache) == base / "cache" / "product");
  CHECK(path(ApplicationDirectory::Runtime) == base / "runtime" / "product");
}

TEST_CASE("Paths has no fallback for the standard runtime application directory", "[paths]")
{
  auto runtime = ScopedEnvironment("XDG_RUNTIME_DIR", std::nullopt);

  CHECK_THROWS_AS(
    application_directory_path(ApplicationDirectory::Runtime, "WCommon", "product"),
    PathError
  );
  auto ec = std::error_code();
  CHECK(application_directory_path(
          ApplicationDirectory::Runtime,
          "WCommon",
          "product",
          ApplicationLayout::Standard,
          ec
  )
          .empty());
  CHECK(ec);
  CHECK_NOTHROW(application_directory_path(ApplicationDirectory::Config, "WCommon", "product"));
}
#endif
