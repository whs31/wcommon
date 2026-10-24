#include <whs31/common/log.h>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>
#include <string_view>

#include <spdlog/async.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qdebug.h>
#endif

namespace
{
  [[nodiscard]] constexpr spdlog::level::level_enum default_log_level() noexcept
  {
#ifdef WHS31_DEBUG
    return spdlog::level::debug;
#else
    return spdlog::level::info;
#endif
  }

  struct LogState
  {
#ifdef WHS31_COMMON_WITH_QT
    QtMessageHandler previous_qt_handler = nullptr;
    std::string target_logger = "qt";
    bool qt_logger_installed = false;
#endif
    std::shared_ptr<spdlog::sinks::dist_sink_mt>
      dynamic_sink_router = std::make_shared<spdlog::sinks::dist_sink_mt>();
  };

  [[nodiscard]] LogState& log_state()
  {
    static auto state = LogState();
    return state;
  }

  [[nodiscard]] bool has_target(
    whs31::common::LogTarget const target,
    whs31::common::LogTarget const flag
  ) noexcept
  {
    return (target & flag) == flag;
  }

  [[nodiscard]] std::size_t file_size_bytes(float const max_file_size_mb)
  {
    if(not std::isfinite(max_file_size_mb) or max_file_size_mb <= 0.0f)
      throw whs31::common::LogError("max_file_size_mb must be finite and greater than zero");

    auto constexpr bytes_per_mebibyte = 1'024.0 * 1'024.0;
    auto const bytes = static_cast<double>(max_file_size_mb) * bytes_per_mebibyte;
    if(bytes > static_cast<double>(std::numeric_limits<std::size_t>::max()))
      throw whs31::common::LogError("max_file_size_mb is too large");

    return static_cast<std::size_t>(bytes);
  }

  void validate_config(whs31::common::LoggerConfig const& config)
  {
    auto constexpr known_targets = std::to_underlying(whs31::common::LogTarget::Stdout)
                                 | std::to_underlying(whs31::common::LogTarget::File);
    auto const target = std::to_underlying(config.target);

    if(config.name.empty())
      throw whs31::common::LogError("logger name must not be empty");
    if(target == 0 or (target & ~known_targets) != 0)
      throw whs31::common::
        LogError("invalid logger target mask '{}'", static_cast<unsigned int>(target));
    for(auto const& sink : config.additional_sinks) {
      if(not sink)
        throw whs31::common::LogError("additional logger sinks must not be null");
    }

    if(has_target(config.target, whs31::common::LogTarget::File)) {
      if(not config.filename or config.filename->empty())
        throw whs31::common::LogError("filename must be non-empty for file target logging");
      if(config.max_files == 0)
        throw whs31::common::LogError("max_files must be greater than zero");
      static_cast<void>(file_size_bytes(config.max_file_size_mb));
    }
  }

#ifdef WHS31_COMMON_WITH_QT
  void message_handler(QtMsgType const type, QMessageLogContext const& ctx, QString const& msg)
  {
    auto const logger = spdlog::get(log_state().target_logger);
    if(not logger)
      return;

    auto const location = spdlog::
      source_loc {ctx.file ? ctx.file : "", ctx.line, ctx.function ? ctx.function : ""};

    auto const* category = ctx.category ? ctx.category : "unknown";

    auto const utf8_msg = msg.toStdString();

    switch(type) {
      case QtDebugMsg:
        logger->log(location, spdlog::level::debug, "[{}] {}", category, utf8_msg);
        break;
      case QtInfoMsg:
        logger->log(location, spdlog::level::info, "[{}] {}", category, utf8_msg);
        break;
      case QtWarningMsg:
        logger->log(location, spdlog::level::warn, "[{}] {}", category, utf8_msg);
        break;
      case QtCriticalMsg:
        logger->log(location, spdlog::level::err, "[{}] {}", category, utf8_msg);
        break;
      case QtFatalMsg:
        logger->log(location, spdlog::level::critical, "[{}] {}", category, utf8_msg);
        break;
    }
  }
#endif
}  // namespace

WHS31_COMMON_NAMESPACE_BEGIN

spdlog::level::level_enum get_env_log_level() noexcept
{
  if(char const* env = std::getenv("LOG_LEVEL")) {
    auto const level = spdlog::level::from_str(env);
    if(level != spdlog::level::off or std::string_view {env} == "off")
      return level;
  }
  return default_log_level();
}

std::shared_ptr<spdlog::logger> create_logger(LoggerConfig const& config)
{
  validate_config(config);

  auto sinks = std::vector<spdlog::sink_ptr>();

  if(has_target(config.target, LogTarget::Stdout))
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

  if(has_target(config.target, LogTarget::File)) {
    auto const folder = config.folder.value_or(std::filesystem::current_path());
    if(not std::filesystem::exists(folder))
      std::filesystem::create_directories(folder);

    auto const base_path = std::filesystem::path(config.filename.value_or(""));
    auto const launch_filename = folder / base_path;

    sinks.push_back(
      std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        launch_filename.string(),
        file_size_bytes(config.max_file_size_mb),
        config.max_files
      )
    );
  }

  if(config.on_warn_or_error) {
    auto const callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>(
      [cb = config.on_warn_or_error](spdlog::details::log_msg const& msg) { cb(msg); }
    );
    callback_sink->set_level(spdlog::level::warn);
    sinks.push_back(callback_sink);
  }
  sinks.insert(sinks.end(), config.additional_sinks.begin(), config.additional_sinks.end());
  sinks.push_back(log_state().dynamic_sink_router);

  auto logger = std::shared_ptr<spdlog::logger>();
  if(config.use_async) {
    if(not spdlog::thread_pool())
      spdlog::init_thread_pool(1'024, 1);
    logger = std::make_shared<spdlog::async_logger>(
      config.name,
      sinks.begin(),
      sinks.end(),
      spdlog::thread_pool(),
      spdlog::async_overflow_policy::block
    );
  }
  else {
    logger = std::make_shared<spdlog::logger>(config.name, sinks.begin(), sinks.end());
  }

  logger->set_level(config.level);
  if(not config.pattern.empty())
    logger->set_pattern(config.pattern);
  logger->flush_on(spdlog::level::debug);

  spdlog::register_logger(logger);
  if(config.is_default)
    spdlog::set_default_logger(logger);

  logger->trace("logger '{}' initialized successfully", config.name);
  return logger;
}

void shutdown_log()
{
#ifdef WHS31_COMMON_WITH_QT
  uninstall_qt_logger();
#endif
  spdlog::shutdown();
}

void attach_log_sink(spdlog::sink_ptr sink)
{
  if(not sink)
    throw LogError("log sink must not be null");

  log_state().dynamic_sink_router->add_sink(std::move(sink));
}

void detach_log_sink(spdlog::sink_ptr const& sink) noexcept
{
  if(sink)
    log_state().dynamic_sink_router->remove_sink(sink);
}

#ifdef WHS31_COMMON_WITH_QT
void install_qt_logger(char const* target_logger_name)
{
  if(target_logger_name == nullptr or *target_logger_name == '\0')
    throw LogError("target logger name must not be null or empty");

  auto& state = log_state();
  state.target_logger = target_logger_name;
  if(not state.qt_logger_installed) {
    state.previous_qt_handler = qInstallMessageHandler(message_handler);
    state.qt_logger_installed = true;
  }

  if(auto const l = spdlog::get(state.target_logger))
    l->trace("installed QtMessageHandler bridging");
}

void uninstall_qt_logger()
{
  auto& state = log_state();
  if(not state.qt_logger_installed)
    return;

  qInstallMessageHandler(state.previous_qt_handler);
  state.previous_qt_handler = nullptr;
  state.qt_logger_installed = false;
}
#endif

WHS31_COMMON_NAMESPACE_END
