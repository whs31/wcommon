#include <radar/mms/common/fatal.h>

#include <cstdio>
#include <cstdlib>

#include <fmt/color.h>

#include <radar/mms/common/log.h>

RADAR_COMMON_NAMESPACE_BEGIN

void fatal(std::string const& message)
{
  shutdown_log();

  fmt::print(
    stderr,
    "{}{}\n",
    fmt::styled("fatal:", fmt::text_style {fmt::emphasis::bold} | fmt::fg(fmt::color::red)),
    fmt::styled(fmt::format(" {}", message), fmt::text_style {fmt::emphasis::bold})
  );
  static_cast<void>(std::fflush(stderr));
  std::exit(EXIT_FAILURE);
}

RADAR_COMMON_NAMESPACE_END
