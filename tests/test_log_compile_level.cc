#define WHS31_LOG_ACTIVE_LEVEL 3  // NOLINT(cppcoreguidelines-macro-usage): configures log.h below
#include <whs31/common/log.h>

#include <string>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

TEST_CASE("Logging macros discard calls below their compile-time level", "[log]")
{
  auto const name = std::string {"wcommon_log_test_compile_level"};
  auto config = LoggerConfig {
    .name = name,
    .level = spdlog::level::trace,
    .target = LogTarget::Stdout,
    .pattern = {}
  };
  auto const logger = create_logger(config);
  auto evaluations = 0;

  WHS31_LOG_TO(name, debug, "discarded argument {}", ++evaluations);
  WHS31_LOG_TO(name, warn, "retained argument {}", ++evaluations);
  WHS31_LOG(debug, "discarded default argument {}", ++evaluations);
  WHS31_LOG(warn, "retained default argument {}", ++evaluations);

  REQUIRE(evaluations == 2);
  spdlog::drop(name);
}
