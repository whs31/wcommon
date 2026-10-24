#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

#include <whs31/common/config.h>
#include <whs31/common/error.h>
#include <whs31/common/detail/namespace.h>
#include <whs31/common/detail/export.h>

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Reads the LOG_LEVEL environment variable as a spdlog level.
 * @details If LOG_LEVEL is unset or invalid, debug builds default to `debug`
 * and release builds default to `info`.
 *
 * @ingroup core_logging
 */
[[nodiscard]] WHS31_COMMON_API spdlog::level::level_enum get_env_log_level() noexcept;

/**
 * @brief Error thrown when logger creation or Qt logger installation fails.
 * @ingroup core_errors
 */
class WHS31_COMMON_API LogError final : public Error
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
 * @brief Fixed-width spdlog pattern for applications that want aligned log columns.
 * @details Produces lines like:
 *
 * `[13:43:05.120] info     helisim.model            parameters.cc:53   loaded VT30 parameters`
 *
 * The level column fits `critical`, the logger name column is 21 characters wide and the source
 * file name is right-aligned to 16 characters. Longer names are not truncated and shift the rest
 * of the line. Messages without a source location leave the file and line columns blank.
 *
 * @code {.cpp}
 * auto logger = whs31::create_logger({
 *   .name = "core",
 *   .pattern = std::string(whs31::aligned_log_pattern),
 * });
 * @endcode
 * @ingroup core_logging
 */
inline auto constexpr aligned_log_pattern = std::string_view {
  "[%T.%e] %^%-8l%$ %-21n %16s:%-4# %v"
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
 * using namespace whs31;
 *
 * auto logger = create_logger({
 *   .name = "core",
 *   .is_default = true,
 *   .target = LogTarget::All,
 *   .filename = "app.log",
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
WHS31_COMMON_API std::shared_ptr<spdlog::logger> create_logger(LoggerConfig const& config);

/**
 * @brief Shuts down spdlog and releases all registered loggers.
 *
 * @ingroup core_logging
 */
WHS31_COMMON_API void shutdown_log();

/**
 * @brief Attaches a sink to all loggers through a thread-safe distributor.
 * @details The sink is routed through a distributor installed during logger construction, so this
 * function can be used while asynchronous loggers are already processing messages. The caller must
 * call detach_log_sink() before destroying objects referenced by @p sink.
 *
 * @throws LogError if @p sink is null.
 * @ingroup core_logging
 */
WHS31_COMMON_API void attach_log_sink(spdlog::sink_ptr sink);

/**
 * @brief Detaches a sink previously attached with attach_log_sink().
 *
 * @ingroup core_logging
 */
WHS31_COMMON_API void detach_log_sink(spdlog::sink_ptr const& sink) noexcept;

#ifdef WHS31_COMMON_WITH_QT
/**
 * @brief Installs a Qt message handler that forwards Qt logs to a spdlog logger.
 * @param target_logger_name Name of the spdlog logger that receives Qt messages.
 * @throws LogError if @p target_logger_name is null or empty.
 * @note Available only when wcommon is built with `with_qt=True`.
 *
 * @ingroup core_logging
 */
WHS31_COMMON_API void install_qt_logger(char const* target_logger_name);

/**
 * @brief Restores the Qt message handler that was active before install_qt_logger().
 * @note Available only when wcommon is built with `with_qt=True`.
 *
 * @ingroup core_logging
 */
WHS31_COMMON_API void uninstall_qt_logger();
#endif

WHS31_COMMON_NAMESPACE_END

[[nodiscard]] constexpr whs31::common::LogTarget operator|(
  whs31::common::LogTarget const lhs,
  whs31::common::LogTarget const rhs
) noexcept
{
  return static_cast<whs31::common::LogTarget>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr whs31::common::LogTarget operator&(
  whs31::common::LogTarget const lhs,
  whs31::common::LogTarget const rhs
) noexcept
{
  return static_cast<whs31::common::LogTarget>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

[[nodiscard]] constexpr whs31::common::LogTarget operator^(
  whs31::common::LogTarget const lhs,
  whs31::common::LogTarget const rhs
) noexcept
{
  return static_cast<whs31::common::LogTarget>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
}

[[nodiscard]] constexpr whs31::common::LogTarget operator~(
  whs31::common::LogTarget const value
) noexcept
{
  return static_cast<whs31::common::LogTarget>(~std::to_underlying(value));
}

[[nodiscard]] constexpr whs31::common::LogTarget& operator|=(
  whs31::common::LogTarget& lhs,
  whs31::common::LogTarget const rhs
) noexcept
{
  lhs = lhs | rhs;
  return lhs;
}

[[nodiscard]] constexpr whs31::common::LogTarget& operator&=(
  whs31::common::LogTarget& lhs,
  whs31::common::LogTarget const rhs
) noexcept
{
  lhs = lhs & rhs;
  return lhs;
}

[[nodiscard]] constexpr whs31::common::LogTarget& operator^=(
  whs31::common::LogTarget& lhs,
  whs31::common::LogTarget const rhs
) noexcept
{
  lhs = lhs ^ rhs;
  return lhs;
}

/**
 * @def WHS31_LOG_LEVEL_TRACE
 * @brief Compile-time trace log level used by WHS31_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef WHS31_LOG_LEVEL_TRACE
#  define WHS31_LOG_LEVEL_TRACE SPDLOG_LEVEL_TRACE
#endif

/**
 * @def WHS31_LOG_LEVEL_DEBUG
 * @brief Compile-time debug log level used by WHS31_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef WHS31_LOG_LEVEL_DEBUG
#  define WHS31_LOG_LEVEL_DEBUG SPDLOG_LEVEL_DEBUG
#endif

/**
 * @def WHS31_LOG_LEVEL_INFO
 * @brief Compile-time info log level used by WHS31_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef WHS31_LOG_LEVEL_INFO
#  define WHS31_LOG_LEVEL_INFO SPDLOG_LEVEL_INFO
#endif

/**
 * @def WHS31_LOG_LEVEL_WARN
 * @brief Compile-time warning log level used by WHS31_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef WHS31_LOG_LEVEL_WARN
#  define WHS31_LOG_LEVEL_WARN SPDLOG_LEVEL_WARN
#endif

/**
 * @def WHS31_LOG_LEVEL_ERROR
 * @brief Compile-time error log level used by WHS31_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef WHS31_LOG_LEVEL_ERROR
#  define WHS31_LOG_LEVEL_ERROR SPDLOG_LEVEL_ERROR
#endif

/**
 * @def WHS31_LOG_LEVEL_CRITICAL
 * @brief Compile-time critical log level used by WHS31_LOG_ACTIVE_LEVEL.
 * @ingroup macros
 */
#ifndef WHS31_LOG_LEVEL_CRITICAL
#  define WHS31_LOG_LEVEL_CRITICAL SPDLOG_LEVEL_CRITICAL
#endif

/**
 * @def WHS31_LOG_ACTIVE_LEVEL
 * @brief Compile-time minimum level for the logging macros.
 * @details Defaults to WHS31_LOG_LEVEL_TRACE so all log calls are compiled in
 * unless a build defines a higher threshold before including this header.
 * @ingroup macros
 */
#ifndef WHS31_LOG_ACTIVE_LEVEL
#  define WHS31_LOG_ACTIVE_LEVEL WHS31_LOG_LEVEL_TRACE
#endif

/**
 * @def WHS31_LOG_TO
 * @brief Logs to a named spdlog logger if it is registered.
 * @param LOGGER Logger name passed to spdlog::get().
 * @param LEVEL spdlog level token, for example trace, debug, info, warn, err,
 * or critical.
 * @param ... fmt-compatible format string and arguments.
 * @details If no logger with @p LOGGER exists, the call is a no-op and format
 * arguments are not evaluated.
 * @ingroup macros
 */
#ifdef WHS31_LOG_TO
#  undef WHS31_LOG_TO
#endif
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define WHS31_LOG_TO(LOGGER, LEVEL, ...)                                                                  \
  do {                                                                                                  \
    if constexpr(spdlog::level::LEVEL >= WHS31_LOG_ACTIVE_LEVEL) {                                      \
      if(std::shared_ptr<spdlog::logger> const l = spdlog::get(LOGGER)) {                               \
        SPDLOG_LOGGER_CALL(l, spdlog::level::LEVEL, __VA_ARGS__);                                       \
      }                                                                                                 \
    }                                                                                                   \
  } while(false)

/**
 * @def WHS31_LOG
 * @brief Logs to spdlog's current default logger.
 * @param LEVEL spdlog level token, for example trace, debug, info, warn, err,
 * or critical.
 * @param ... fmt-compatible format string and arguments.
 * @details The default logger is expected to exist. create_logger() installs
 * one when LoggerConfig::is_default is true; otherwise spdlog's own default
 * logger is used.
 * @ingroup macros
 */
#ifdef WHS31_LOG
#  undef WHS31_LOG
#endif
#define WHS31_LOG(LEVEL, ...)                                                                             \
  do {                                                                                                  \
    if constexpr(spdlog::level::LEVEL >= WHS31_LOG_ACTIVE_LEVEL) {                                      \
      SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::LEVEL, __VA_ARGS__);               \
    }                                                                                                   \
  } while(false)

/**
 * @def WHS31_LOG_FLUSH
 * @brief Flushes a named spdlog logger if it is registered.
 * @param LOGGER Logger name passed to spdlog::get().
 * @ingroup macros
 */
#ifdef WHS31_LOG_FLUSH
#  undef WHS31_LOG_FLUSH
#endif
#define WHS31_LOG_FLUSH(LOGGER)                                                                           \
  do {                                                                                                  \
    if(std::shared_ptr<spdlog::logger> const l = spdlog::get(LOGGER)) {                                 \
      l->flush();                                                                                       \
    }                                                                                                   \
  } while(false)
// NOLINTEND(cppcoreguidelines-macro-usage)
