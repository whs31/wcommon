#include <whs31/common/version.h>

#include <array>
#include <compare>
#include <limits>
#include <ranges>
#include <sstream>
#include <string_view>
#include <unordered_set>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

TEST_CASE("Version parses and formats semantic versions", "[version]")
{
  auto const version = Version("0.8.1-alpha.1+zstd.1.5.0");

  CHECK(version.major() == 0);
  CHECK(version.minor() == 8);
  CHECK(version.patch() == 1);
  CHECK(version.pre().as_str() == "alpha.1");
  CHECK(version.build().as_str() == "zstd.1.5.0");
  CHECK(version.to_string() == "0.8.1-alpha.1+zstd.1.5.0");
  STATIC_CHECK(Version(1, 2, 3).major() == 1);
}

TEST_CASE("Version rejects malformed and overflowing input", "[version]")
{
  for(auto const text : {
        std::string_view {},
        std::string_view {"1.0"},
        std::string_view {"1.0.0.0"},
        std::string_view {"1.01.0"},
        std::string_view {"1.0.0-"},
        std::string_view {"1.0.0+"},
        std::string_view {"1.0.0-alpha_1"},
        std::string_view {"1.0.0-alpha.01"},
        std::string_view {"18446744073709551616.0.0"},
        std::string_view {"1.2.3\0suffix", 12},
      }) {
    CAPTURE(text);
    CHECK_THROWS_AS(Version(text), VersionError);
  }
}

TEST_CASE("Version validates identifiers", "[version]")
{
  CHECK(Prerelease("alpha.1").as_str() == "alpha.1");
  CHECK(BuildMetadata("001.sha-abcdef").as_str() == "001.sha-abcdef");
  CHECK_THROWS_AS(Prerelease("alpha..1"), VersionError);
  CHECK_THROWS_AS(Prerelease("01"), VersionError);
  CHECK_THROWS_AS(BuildMetadata("build..1"), VersionError);
  CHECK_THROWS_AS(BuildMetadata("sha_abcdef"), VersionError);
}

TEST_CASE("Version follows SemVer precedence and has a stable total order", "[version]")
{
  auto const ordered = std::array {
    Version("1.0.0-alpha"),
    Version("1.0.0-alpha.1"),
    Version("1.0.0-alpha.beta"),
    Version("1.0.0-beta"),
    Version("1.0.0-beta.2"),
    Version("1.0.0-beta.11"),
    Version("1.0.0-rc.1"),
    Version("1.0.0"),
  };
  for(auto const& [lower, higher] : ordered | std::views::pairwise)
    CHECK(lower.cmp_precedence(higher) == std::strong_ordering::less);

  auto const build_a = Version("1.0.0+a");
  auto const build_b = Version("1.0.0+b");
  CHECK(build_a.cmp_precedence(build_b) == std::strong_ordering::equal);
  CHECK(build_a < build_b);
}

TEST_CASE("Version supports literals streams fmt and hash", "[version]")
{
  auto const version = "1.2.3-alpha.1+build.5"_version;
  auto stream = std::ostringstream();
  stream << version;

  CHECK(stream.str() == "1.2.3-alpha.1+build.5");
  CHECK(fmt::format("{}", version) == stream.str());
  CHECK(std::unordered_set<Version> {version, Version(stream.str())}.size() == 1);
}
