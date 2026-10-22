#define RADAR_LOG_ACTIVE_LEVEL 3  // NOLINT(cppcoreguidelines-macro-usage): configures log.h below
#include <radar/mms/common/log.h>

#include <string>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("Logging macros discard calls below their compile-time level", "[log]")
{
  auto const name = std::string {"rcommon_log_test_compile_level"};
  auto config = LoggerConfig {
    .name = name,
    .level = spdlog::level::trace,
    .target = LogTarget::Stdout,
    .pattern = {}
  };
  auto const logger = create_logger(config);
  auto evaluations = 0;

  RADAR_LOG_TO(name, debug, "discarded argument {}", ++evaluations);
  RADAR_LOG_TO(name, warn, "retained argument {}", ++evaluations);
  RADAR_LOG(debug, "discarded default argument {}", ++evaluations);
  RADAR_LOG(warn, "retained default argument {}", ++evaluations);

  REQUIRE(evaluations == 2);
  spdlog::drop(name);
}
