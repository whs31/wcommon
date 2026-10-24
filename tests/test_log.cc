#include <whs31/common/log.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <limits>
#include <optional>
#include <sstream>
#include <string>

#ifdef WHS31_COMMON_WITH_QT
#  include <qdebug.h>
#endif

#include <catch2/catch_test_macros.hpp>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/ostream_sink.h>

using namespace whs31;

namespace
{
  [[nodiscard]] LoggerConfig stdout_config(std::string name)
  {
    return LoggerConfig {
      .name = std::move(name),
      .level = spdlog::level::trace,
      .target = LogTarget::Stdout,
      .pattern = {}
    };
  }

  class ScopedSpdlogDrop
  {
   public:
    explicit ScopedSpdlogDrop(std::string name)
      : m_name(std::move(name))
    {}

    ~ScopedSpdlogDrop() { spdlog::drop(this->m_name); }

    ScopedSpdlogDrop(ScopedSpdlogDrop const&) = delete;
    ScopedSpdlogDrop& operator=(ScopedSpdlogDrop const&) = delete;
    ScopedSpdlogDrop(ScopedSpdlogDrop&&) = delete;
    ScopedSpdlogDrop& operator=(ScopedSpdlogDrop&&) = delete;

   private:
    std::string m_name;
  };

  class ScopedEnvironment
  {
   public:
    ScopedEnvironment(char const* const name, std::optional<std::string> value)
      : m_name(name)
    {
      if(auto const* previous = std::getenv(name); previous != nullptr)
        this->m_previous = previous;
#ifdef _WIN32
      static_cast<void>(::_putenv_s(name, value ? value->c_str() : ""));
#else
      if(value)
        REQUIRE(::setenv(name, value->c_str(), 1) == 0);
      else
        REQUIRE(::unsetenv(name) == 0);
#endif
    }

    ~ScopedEnvironment()
    {
#ifdef _WIN32
      static_cast<void>(::_putenv_s(this->m_name.c_str(), this->m_previous.value_or("").c_str()));
#else
      if(this->m_previous)
        static_cast<void>(::setenv(this->m_name.c_str(), this->m_previous->c_str(), 1));
      else
        static_cast<void>(::unsetenv(this->m_name.c_str()));
#endif
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

TEST_CASE("LogTarget flags support constexpr bitwise operations", "[log]")
{
  STATIC_CHECK((LogTarget::Stdout | LogTarget::File) == LogTarget::All);
  STATIC_CHECK((LogTarget::All & LogTarget::File) == LogTarget::File);
  STATIC_CHECK((LogTarget::All ^ LogTarget::File) == LogTarget::Stdout);

  auto target = LogTarget::Stdout;
  STATIC_CHECK((~LogTarget::Stdout & LogTarget::All) == LogTarget::File);
  REQUIRE((target |= LogTarget::File) == LogTarget::All);
  REQUIRE((target &= LogTarget::File) == LogTarget::File);
  REQUIRE((target ^= LogTarget::All) == LogTarget::Stdout);
}

TEST_CASE("Environment controls the default log level", "[log]")
{
  SECTION("explicit value")
  {
    auto env = ScopedEnvironment("LOG_LEVEL", "trace");
    REQUIRE(get_env_log_level() == spdlog::level::trace);
  }

  SECTION("build default")
  {
    auto env = ScopedEnvironment("LOG_LEVEL", std::nullopt);
#ifdef WHS31_DEBUG
    REQUIRE(get_env_log_level() == spdlog::level::debug);
#else
    REQUIRE(get_env_log_level() == spdlog::level::info);
#endif
  }

  SECTION("invalid value falls back to the build default")
  {
    auto env = ScopedEnvironment("LOG_LEVEL", "verbose");
#ifdef WHS31_DEBUG
    REQUIRE(get_env_log_level() == spdlog::level::debug);
#else
    REQUIRE(get_env_log_level() == spdlog::level::info);
#endif
  }
}

TEST_CASE("create_logger creates and registers a stdout logger", "[log]")
{
  auto const name = std::string {"wcommon_log_test_stdout"};
  auto const drop = ScopedSpdlogDrop(name);

  auto const logger = create_logger(stdout_config(name));

  REQUIRE(logger != nullptr);
  REQUIRE(logger->name() == name);
  REQUIRE(spdlog::get(name) == logger);
}

TEST_CASE("create_logger invokes callback sink only for warnings and errors", "[log]")
{
  auto const name = std::string {"wcommon_log_test_callback"};
  auto const drop = ScopedSpdlogDrop(name);
  auto callback_count = std::atomic_int {0};
  auto config = stdout_config(name);
  config.on_warn_or_error = [&](auto const&) { ++callback_count; };

  auto const logger = create_logger(config);
  logger->debug("debug does not reach callback sink");
  logger->warn("warning reaches callback sink");
  logger->error("error reaches callback sink");

  REQUIRE(callback_count == 2);
}

TEST_CASE("create_logger installs additional sinks during construction", "[log]")
{
  auto const name = std::string {"wcommon_log_test_additional_sink"};
  auto const drop = ScopedSpdlogDrop(name);
  auto callback_count = std::atomic_int {0};
  auto config = stdout_config(name);
  config.additional_sinks.push_back(
    std::make_shared<spdlog::sinks::callback_sink_mt>([&](auto const&) { ++callback_count; })
  );

  auto const logger = create_logger(config);
  callback_count = 0;
  logger->info("info reaches additional sink");

  REQUIRE(callback_count == 1);
}

TEST_CASE("aligned_log_pattern pads every column to a fixed width", "[log]")
{
  auto const name = std::string {"wcommon.pattern"};
  auto const drop = ScopedSpdlogDrop(name);
  auto stream = std::ostringstream();
  auto config = stdout_config(name);
  config.pattern = std::string(aligned_log_pattern);
  config.additional_sinks.push_back(std::make_shared<spdlog::sinks::ostream_sink_mt>(stream));

  auto const logger = create_logger(config);
  stream.str({});
  logger->log(
    spdlog::source_loc {"/src/model/parameters.cc", 53, "load"},
    spdlog::level::info,
    "loaded parameters"
  );
  logger->log(spdlog::level::critical, "no source");

  auto lines = std::istringstream(stream.str());
  auto with_source = std::string();
  auto without_source = std::string();
  REQUIRE(std::getline(lines, with_source));
  REQUIRE(std::getline(lines, without_source));

  // `[13:43:05.120] ` is 15 characters wide
  REQUIRE(
    with_source.substr(15)
    == "info     wcommon.pattern          parameters.cc:53   loaded parameters"
  );
  REQUIRE(
    without_source.substr(15) == "critical wcommon.pattern                       :     no source"
  );
}

TEST_CASE("Dynamic log sinks can be attached and detached at runtime", "[log]")
{
  auto const name = std::string {"wcommon_log_test_dynamic_sink"};
  auto const drop = ScopedSpdlogDrop(name);
  auto callback_count = std::atomic_int {0};

  auto const logger = create_logger(stdout_config(name));
  auto const sink = std::make_shared<spdlog::sinks::callback_sink_mt>(
    [&callback_count](auto const&) { ++callback_count; }
  );

  attach_log_sink(sink);
  logger->info("info reaches dynamic sink");
  logger->flush();

  detach_log_sink(sink);
  logger->warn("warning does not reach detached dynamic sink");
  logger->flush();

  REQUIRE(callback_count == 1);
  REQUIRE_THROWS_AS(attach_log_sink(nullptr), LogError);
}

TEST_CASE("Logging macros target registered loggers", "[log]")
{
  auto const name = std::string {"wcommon_log_test_macros"};
  auto const drop = ScopedSpdlogDrop(name);
  auto callback_count = std::atomic_int {0};
  auto config = stdout_config(name);
  config.on_warn_or_error = [&](auto const&) { ++callback_count; };

  auto const logger = create_logger(config);
  static_cast<void>(logger);

  WHS31_LOG_TO(name, warn, "macro warning reaches callback sink");
  WHS31_LOG_TO("wcommon_log_test_missing", warn, "missing logger is ignored");
  WHS31_LOG_FLUSH(name);
  WHS31_LOG_FLUSH("wcommon_log_test_missing");

  REQUIRE(callback_count == 1);
}

TEST_CASE("create_logger validates configuration", "[log]")
{
  SECTION("empty logger name")
  {
    auto config = stdout_config({});
    REQUIRE_THROWS_AS(create_logger(config), LogError);
  }

  SECTION("invalid target mask")
  {
    auto config = stdout_config("wcommon_log_test_invalid_mask");
    config.target = static_cast<LogTarget>(4);
    REQUIRE_THROWS_AS(create_logger(config), LogError);
  }

  SECTION("file target without filename")
  {
    auto config = stdout_config("wcommon_log_test_missing_filename");
    config.target = LogTarget::File;
    REQUIRE_THROWS_AS(create_logger(config), LogError);
  }

  SECTION("file target with empty filename")
  {
    auto config = stdout_config("wcommon_log_test_empty_filename");
    config.target = LogTarget::File;
    config.filename = "";
    REQUIRE_THROWS_AS(create_logger(config), LogError);
  }

  SECTION("non-positive file size")
  {
    auto config = stdout_config("wcommon_log_test_bad_size");
    config.target = LogTarget::File;
    config.filename = "bad-size.log";
    config.max_file_size_mb = 0.0f;
    REQUIRE_THROWS_AS(create_logger(config), LogError);
  }

  SECTION("non-finite file size")
  {
    auto config = stdout_config("wcommon_log_test_non_finite_size");
    config.target = LogTarget::File;
    config.filename = "bad-size.log";

    config.max_file_size_mb = std::numeric_limits<float>::infinity();
    REQUIRE_THROWS_AS(create_logger(config), LogError);

    config.max_file_size_mb = std::numeric_limits<float>::quiet_NaN();
    REQUIRE_THROWS_AS(create_logger(config), LogError);
  }

  SECTION("zero retained files")
  {
    auto config = stdout_config("wcommon_log_test_bad_count");
    config.target = LogTarget::File;
    config.filename = "bad-count.log";
    config.max_files = 0;
    REQUIRE_THROWS_AS(create_logger(config), LogError);
  }

  SECTION("null additional sink")
  {
    auto config = stdout_config("wcommon_log_test_null_additional_sink");
    config.additional_sinks.emplace_back();
    REQUIRE_THROWS_AS(create_logger(config), LogError);
  }
}

TEST_CASE("create_logger creates rotating file sink folders", "[log]")
{
  auto const name = std::string {"wcommon_log_test_file"};
  auto const drop = ScopedSpdlogDrop(name);
  auto const folder = std::filesystem::temp_directory_path() / "wcommon_log_tests";
  auto const filename = std::string {"file.log"};
  std::filesystem::remove_all(folder);
  auto config = LoggerConfig {
    .name = name,
    .level = spdlog::level::trace,
    .target = LogTarget::File,
    .pattern = {},
    .filename = filename,
    .folder = folder,
    .max_file_size_mb = 1.0f,
    .max_files = 2
  };

  auto const logger = create_logger(config);
  logger->info("file logger smoke test");
  logger->flush();

  REQUIRE(std::filesystem::exists(folder));
  REQUIRE(std::filesystem::exists(folder / filename));
  std::filesystem::remove_all(folder);
}

#ifdef WHS31_COMMON_WITH_QT
TEST_CASE("Qt message handler owns its target logger name", "[log][qt]")
{
  auto const name = std::string {"wcommon_log_test_qt_owned_name"};
  auto const drop = ScopedSpdlogDrop(name);
  auto callback_count = std::atomic_int {0};
  auto config = stdout_config(name);
  config.on_warn_or_error = [&](auto const&) { ++callback_count; };
  auto const logger = create_logger(config);
  static_cast<void>(logger);

  auto mutable_name = name;
  install_qt_logger(mutable_name.c_str());
  std::ranges::fill(mutable_name, 'x');
  qWarning("bridged Qt warning");
  uninstall_qt_logger();

  REQUIRE(callback_count == 1);
}

TEST_CASE("Qt logger installation is idempotent and validates its target", "[log][qt]")
{
  auto const name = std::string {"wcommon_log_test_qt_idempotent"};
  auto const drop = ScopedSpdlogDrop(name);
  auto const logger = create_logger(stdout_config(name));
  static_cast<void>(logger);

  install_qt_logger(name.c_str());
  install_qt_logger(name.c_str());
  uninstall_qt_logger();
  uninstall_qt_logger();

  REQUIRE_THROWS_AS(install_qt_logger(nullptr), LogError);
  REQUIRE_THROWS_AS(install_qt_logger(""), LogError);
}

TEST_CASE("Logging shutdown uninstalls the Qt message handler", "[log][qt]")
{
  auto const name = std::string {"wcommon_log_test_qt_shutdown"};
  auto callback_count = std::atomic_int {0};
  auto config = stdout_config(name);
  config.on_warn_or_error = [&](auto const&) { ++callback_count; };
  auto const logger = create_logger(config);
  static_cast<void>(logger);

  install_qt_logger(name.c_str());
  shutdown_log();
  qWarning("warning after shutdown");
  spdlog::set_default_logger(std::make_shared<spdlog::logger>(""));

  REQUIRE(callback_count == 0);
}
#endif
