#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

#include <radar/mms/common/config.h>
#include <radar/mms/common/error.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/detail/export.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Reads the LOG_LEVEL environment variable as a spdlog level.
 * @details If LOG_LEVEL is unset or invalid, debug builds default to `debug`
 * and release builds default to `info`.
 *
 * @ingroup core_logging
 */
[[nodiscard]] RADAR_COMMON_API spdlog::level::level_enum get_env_log_level() noexcept;

/**
 * @brief Error thrown when logger creation or Qt logger installation fails.
 * @ingroup core_errors
 */
class RADAR_COMMON_API LogError final : public Error
{
 public:
  template <typename... Args>
  explicit LogError(fmt::format_string<Args...> fmt, Args&&... args)
    : Error(fmt, std::forward<Args>(args)...)
  {}
};

/**
 * @brief Sink target bitmask for create_logger().
 * @ingroup core_logging
 */
enum class LogTarget : std::uint8_t
{
  Stdout = 1 << 0,           ///< Add a colored stdout sink.
  File = 1 << 1,             ///< Add a rotating file sink.
  All = (1 << 0) | (1 << 1)  ///< Add stdout and file sinks.
};

/**
 * @brief Configuration passed to create_logger().
 * @details File logging is enabled by setting target to LogTarget::File or
 * LogTarget::All and providing filename. The optional folder defaults to the
 * current working directory. File sinks use spdlog's rotating file sink and
 * honor max_file_size_mb and max_files.
 *
 * The on_warn_or_error callback, when set, receives messages at warning level
 * and above through a dedicated callback sink. It is useful for UI diagnostics
 * and counters.
 *
 * Additional sinks must be supplied through additional_sinks during logger
 * creation. Mutating a logger's sinks after an asynchronous logger starts
 * processing messages is not thread-safe.
 *
 * @code {.cpp}
 * using namespace radar::mms;
 *
 * auto logger = create_logger({
 *   .name = "core",
 *   .is_default = true,
 *   .target = LogTarget::All,
 *   .filename = "mms.log",
 *   .folder = app_logs_dir,
 *   .on_warn_or_error = [](spdlog::details::log_msg const& msg) {
 *     count_warning_or_error(msg);
 *   },
 * });
 *
 * logger->info("logging initialized");
 * @endcode
 * @ingroup core_logging
 */
struct LoggerConfig
{
  /**
   * @brief Name used in spdlog's global registry.
   */
  std::string name;

  /**
   * @brief Install the created logger as spdlog's default logger.
   */
  bool is_default = false;

  /**
   * @brief Create an asynchronous logger using spdlog's global thread pool.
   */
  bool use_async = false;

  /**
   * @brief Minimum level accepted by the logger.
   */
  spdlog::level::level_enum level = get_env_log_level();

  /**
   * @brief Output targets to create.
   */
  LogTarget target = LogTarget::All;

  /**
   * @brief spdlog pattern applied to all sinks. Empty keeps spdlog's default pattern.
   */
  std::string pattern = "[%X] (%n) [%^%l%$] <%-15s %-15! line %-4#> %^%v%$";

  /**
   * @brief Filename used by the rotating file sink.
   */
  std::optional<std::string> filename;

  /**
   * @brief Folder used by the rotating file sink.
   */
  std::optional<std::filesystem::path> folder;

  /**
   * @brief Maximum size of a log file before rotation, in MiB.
   */
  float max_file_size_mb = 10.0f;

  /**
   * @brief Number of rotated files to retain.
   */
  std::size_t max_files = 3;

  /**
   * @brief Optional callback sink invoked for warning, error, and critical messages.
   */
  std::function<void(spdlog::details::log_msg const&)> on_warn_or_error = nullptr;

  /**
   * @brief Additional sinks installed before the logger starts processing messages.
   */
  std::vector<spdlog::sink_ptr> additional_sinks;
};

/**
 * @brief Creates and registers a configured spdlog logger.
 * @param config Logger configuration.
 * @return Shared logger registered with spdlog.
 * @throws LogError if the configuration is invalid.
 * @throws spdlog::spdlog_ex if spdlog fails to create or register the logger.
 *
 * @ingroup core_logging
 */
RADAR_COMMON_API std::shared_ptr<spdlog::logger> create_logger(LoggerConfig const& config);

/**
 * @brief Shuts down spdlog and releases all registered loggers.
 *
 * @ingroup core_logging
 */
RADAR_COMMON_API void shutdown_log();

/**
 * @brief Attaches a sink to all MMS loggers through a thread-safe distributor.
 * @details The sink is routed through a distributor installed during logger construction, so this
 * function can be used while asynchronous loggers are already processing messages. The caller must
 * call detach_log_sink() before destroying objects referenced by @p sink.
 *
 * @throws LogError if @p sink is null.
 * @ingroup core_logging
 */
RADAR_COMMON_API void attach_log_sink(spdlog::sink_ptr sink);

/**
 * @brief Detaches a sink previously attached with attach_log_sink().
 *
 * @ingroup core_logging
 */
RADAR_COMMON_API void detach_log_sink(spdlog::sink_ptr const& sink) noexcept;

#ifdef RCOMMON_WITH_QT
/**
 * @brief Installs a Qt message handler that forwards Qt logs to a spdlog logger.
 * @param target_logger_name Name of the spdlog logger that receives Qt messages.
 * @throws LogError if @p target_logger_name is null or empty.
 * @note Available only when rcommon is built with `with_qt=True`.
 *
 * @ingroup core_logging
 */
RADAR_COMMON_API void install_qt_logger(char const* target_logger_name);

/**
 * @brief Restores the Qt message handler that was active before install_qt_logger().
 * @note Available only when rcommon is built with `with_qt=True`.
 *
 * @ingroup core_logging
 */
RADAR_COMMON_API void uninstall_qt_logger();
#endif

RADAR_COMMON_NAMESPACE_END

[[nodiscard]] constexpr radar::mms::common::LogTarget operator|(
  radar::mms::common::LogTarget const lhs,
  radar::mms::common::LogTarget const rhs
) noexcept
{
  return static_cast<radar::mms::common::LogTarget>(
    std::to_underlying(lhs) | std::to_underlying(rhs)
  );
}

[[nodiscard]] constexpr radar::mms::common::LogTarget operator&(
  radar::mms::common::LogTarget const lhs,
  radar::mms::common::LogTarget const rhs
) noexcept
{
  return static_cast<radar::mms::common::LogTarget>(
    std::to_underlying(lhs) & std::to_underlying(rhs)
  );
}

[[nodiscard]] constexpr radar::mms::common::LogTarget operator^(
  radar::mms::common::LogTarget const lhs,
  radar::mms::common::LogTarget const rhs
) noexcept
{
  return static_cast<radar::mms::common::LogTarget>(
    std::to_underlying(lhs) ^ std::to_underlying(rhs)
  );
}

[[nodiscard]] constexpr radar::mms::common::LogTarget operator~(
  radar::mms::common::LogTarget const value
) noexcept
{
  return static_cast<radar::mms::common::LogTarget>(~std::to_underlying(value));
}

[[nodiscard]] constexpr radar::mms::common::LogTarget& operator|=(
  radar::mms::common::LogTarget& lhs,
  radar::mms::common::LogTarget const rhs
) noexcept
{
  lhs = lhs | rhs;
  return lhs;
}

[[nodiscard]] constexpr radar::mms::common::LogTarget& operator&=(
  radar::mms::common::LogTarget& lhs,
  radar::mms::common::LogTarget const rhs
) noexcept
{
  lhs = lhs & rhs;
  return lhs;
}

[[nodiscard]] constexpr radar::mms::common::LogTarget& operator^=(
  radar::mms::common::LogTarget& lhs,
  radar::mms::common::LogTarget const rhs
) noexcept
{
  lhs = lhs ^ rhs;
  return lhs;
}

/**
 * @def RADAR_LOG_LEVEL_TRACE
 * @brief Compile-time trace log level used by RADAR_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef RADAR_LOG_LEVEL_TRACE
#  define RADAR_LOG_LEVEL_TRACE SPDLOG_LEVEL_TRACE
#endif

/**
 * @def RADAR_LOG_LEVEL_DEBUG
 * @brief Compile-time debug log level used by RADAR_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef RADAR_LOG_LEVEL_DEBUG
#  define RADAR_LOG_LEVEL_DEBUG SPDLOG_LEVEL_DEBUG
#endif

/**
 * @def RADAR_LOG_LEVEL_INFO
 * @brief Compile-time info log level used by RADAR_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef RADAR_LOG_LEVEL_INFO
#  define RADAR_LOG_LEVEL_INFO SPDLOG_LEVEL_INFO
#endif

/**
 * @def RADAR_LOG_LEVEL_WARN
 * @brief Compile-time warning log level used by RADAR_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef RADAR_LOG_LEVEL_WARN
#  define RADAR_LOG_LEVEL_WARN SPDLOG_LEVEL_WARN
#endif

/**
 * @def RADAR_LOG_LEVEL_ERROR
 * @brief Compile-time error log level used by RADAR_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef RADAR_LOG_LEVEL_ERROR
#  define RADAR_LOG_LEVEL_ERROR SPDLOG_LEVEL_ERROR
#endif

/**
 * @def RADAR_LOG_LEVEL_CRITICAL
 * @brief Compile-time critical log level used by RADAR_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef RADAR_LOG_LEVEL_CRITICAL
#  define RADAR_LOG_LEVEL_CRITICAL SPDLOG_LEVEL_CRITICAL
#endif

/**
 * @def RADAR_LOG_ACTIVE_LEVEL
 * @brief Compile-time minimum level for MMS logging macros.
 * @details Defaults to RADAR_LOG_LEVEL_TRACE so all log calls are compiled in
 * unless a build defines a higher threshold before including this header.
 * @ingroup macros
 */
#ifndef RADAR_LOG_ACTIVE_LEVEL
#  define RADAR_LOG_ACTIVE_LEVEL RADAR_LOG_LEVEL_TRACE
#endif

/**
 * @def RADAR_LOG_TO
 * @brief Logs to a named spdlog logger if it is registered.
 * @param LOGGER Logger name passed to spdlog::get().
 * @param LEVEL spdlog level token, for example trace, debug, info, warn, err,
 * or critical.
 * @param ... fmt-compatible format string and arguments.
 * @details If no logger with @p LOGGER exists, the call is a no-op and format
 * arguments are not evaluated.
 * @ingroup macros
 */
#ifdef RADAR_LOG_TO
#  undef RADAR_LOG_TO
#endif
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define RADAR_LOG_TO(LOGGER, LEVEL, ...)                                                                  \
  do {                                                                                                  \
    if constexpr(spdlog::level::LEVEL >= RADAR_LOG_ACTIVE_LEVEL) {                                      \
      if(std::shared_ptr<spdlog::logger> const l = spdlog::get(LOGGER)) {                               \
        SPDLOG_LOGGER_CALL(l, spdlog::level::LEVEL, __VA_ARGS__);                                       \
      }                                                                                                 \
    }                                                                                                   \
  } while(false)

/**
 * @def RADAR_LOG
 * @brief Logs to spdlog's current default logger.
 * @param LEVEL spdlog level token, for example trace, debug, info, warn, err,
 * or critical.
 * @param ... fmt-compatible format string and arguments.
 * @details The default logger is expected to exist. create_logger() installs
 * one when LoggerConfig::is_default is true; otherwise spdlog's own default
 * logger is used.
 * @ingroup macros
 */
#ifdef RADAR_LOG
#  undef RADAR_LOG
#endif
#define RADAR_LOG(LEVEL, ...)                                                                             \
  do {                                                                                                  \
    if constexpr(spdlog::level::LEVEL >= RADAR_LOG_ACTIVE_LEVEL) {                                      \
      SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::LEVEL, __VA_ARGS__);               \
    }                                                                                                   \
  } while(false)

/**
 * @def RADAR_LOG_FLUSH
 * @brief Flushes a named spdlog logger if it is registered.
 * @param LOGGER Logger name passed to spdlog::get().
 * @ingroup macros
 */
#ifdef RADAR_LOG_FLUSH
#  undef RADAR_LOG_FLUSH
#endif
#define RADAR_LOG_FLUSH(LOGGER)                                                                           \
  do {                                                                                                  \
    if(std::shared_ptr<spdlog::logger> l = spdlog::get(LOGGER)) {                                       \
      l->flush();                                                                                       \
    }                                                                                                   \
  } while(false)
// NOLINTEND(cppcoreguidelines-macro-usage)
