#include <whs31/common/glaze/chrono.h>

#include <chrono>
#include <cstdint>
#include <string>

#include <glaze/beve.hpp>
#include <glaze/json.hpp>
#include <glaze/toml.hpp>

#include <catch2/catch_test_macros.hpp>

#include "glaze_support.h"

using namespace glaze_support;
using namespace std::chrono_literals;

namespace
{
  using Milliseconds = std::chrono::duration<double, std::milli>;
}  // namespace

// glaze reflects on the names of the members, which clang rejects for a type without linkage, so
// this stays out of the anonymous namespace
namespace chrono_glaze_test
{
  struct Timing  // NOLINT(misc-use-internal-linkage)
  {
    Milliseconds latency;
    std::chrono::seconds timeout;
    std::chrono::microseconds period;

    [[nodiscard]] bool operator==(Timing const&) const = default;
  };
}  // namespace chrono_glaze_test

using chrono_glaze_test::Timing;

TEST_CASE("Duration glaze adapter writes and reads the count", "[glaze][chrono]")
{
  check_round_trip(42s, "42");
  check_round_trip(Milliseconds(7.5), "7.5");
  check_round_trip(std::chrono::duration<std::uint16_t, std::milli>(65'535), "65535");
  check_round_trip(
    Timing {.latency = 2.5ms, .timeout = 30s, .period = 20'000us},
    R"({"latency":2.5,"timeout":30,"period":20000})"
  );
  REQUIRE(read_error<std::chrono::seconds>("1.5") != glz::error_code::none);
}

TEST_CASE("Duration glaze adapter reads literal strings", "[glaze][chrono]")
{
  CHECK(read<Milliseconds>(R"("7.5ms")") == 7.5ms);
  CHECK(read<Milliseconds>(R"("-250us")") == -0.25ms);
  CHECK(read<Milliseconds>(R"("25e5ns")") == 2.5ms);
  CHECK(read<std::chrono::milliseconds>(R"("1.5s")") == 1'500ms);
  CHECK(read<std::chrono::seconds>(R"("2min")") == 120s);
  CHECK(read<std::chrono::minutes>(R"("1.5h")") == 90min);
  CHECK(read<std::chrono::duration<double>>(R"("90min")") == 1.5h);

  CHECK(read_error<Milliseconds>(R"("ms")") == glz::error_code::constraint_violated);
  CHECK(read_error<Milliseconds>(R"("7.5")") == glz::error_code::constraint_violated);
  CHECK(read_error<Milliseconds>(R"("7.5 ms")") == glz::error_code::constraint_violated);
  CHECK(read_error<Milliseconds>(R"("7.5d")") == glz::error_code::constraint_violated);
}

TEST_CASE("Duration glaze adapter reads one unit key", "[glaze][chrono]")
{
  CHECK(read<Milliseconds>(R"({"seconds":0.25})") == 250ms);
  CHECK(read<std::chrono::nanoseconds>(R"({"microseconds":3})") == 3'000ns);
  CHECK(read<std::chrono::hours>(R"({"minutes":120})") == 2h);

  CHECK(read_error<Milliseconds>("{}") == glz::error_code::constraint_violated);
  CHECK(
    read_error<Milliseconds>(R"({"seconds":1,"milliseconds":2})")
    == glz::error_code::constraint_violated
  );
  CHECK(read_error<Milliseconds>(R"({"fortnights":1})") == glz::error_code::unknown_key);
}

TEST_CASE("Duration glaze adapter keeps integer counts exact", "[glaze][chrono]")
{
  CHECK(
    read_error<std::chrono::milliseconds>(R"("1500us")") == glz::error_code::constraint_violated
  );
  CHECK(
    read_error<std::chrono::hours>(R"({"minutes":90})") == glz::error_code::constraint_violated
  );
  CHECK(
    read_error<std::chrono::duration<std::uint8_t>>(R"("256s")")
    == glz::error_code::constraint_violated
  );
  CHECK(
    read_error<std::chrono::duration<std::uint8_t>>(R"("-1s")")
    == glz::error_code::constraint_violated
  );
  CHECK(read<std::chrono::duration<std::uint8_t>>(R"("255s")").count() == 255);
  CHECK(
    read_error<std::chrono::nanoseconds>(R"("1e300h")") == glz::error_code::constraint_violated
  );
}

TEST_CASE("Duration glaze adapter reads TOML in natural units", "[glaze][chrono][toml]")
{
  auto timing = Timing {};
  auto const toml = std::string(R"(latency = "7.5ms"
timeout = { minutes = 2.0 }
period = 20000
)");
  auto const error = glz::read_toml(timing, toml);
  INFO(glz::format_error(error, toml));
  REQUIRE_FALSE(error);
  CHECK(timing.latency == 7.5ms);
  CHECK(timing.timeout == 120s);
  CHECK(timing.period == 20ms);
}

TEST_CASE("Duration glaze adapter round-trips BEVE as the count", "[glaze][chrono][beve]")
{
  auto const timing = Timing {.latency = 2.5ms, .timeout = 30s, .period = 20'000us};
  auto buffer = std::string {};
  REQUIRE_FALSE(glz::write_beve(timing, buffer));
  auto back = Timing {};
  REQUIRE_FALSE(glz::read_beve(back, buffer));
  CHECK(back == timing);
}
