#include <radar/mms/common/version_range.h>

#include <array>
#include <sstream>
#include <string_view>
#include <unordered_set>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("VersionRange matches comparators alternatives and partial versions", "[version_range]")
{
  CHECK(VersionRange("1.2.3").contains(Version("1.2.3")));
  CHECK_FALSE(VersionRange("1.2.3").contains(Version("1.2.4")));
  CHECK(VersionRange(">=1.2.3 <2.0.0").contains(Version("1.9.9")));
  CHECK(VersionRange("1.x").contains(Version("1.9.0")));
  CHECK(VersionRange("1.2").contains(Version("1.2.99")));
  CHECK(VersionRange("1.2.3 || >=2.0.0").contains(Version("2.1.0")));
}

TEST_CASE("VersionRange implements tilde caret and hyphen bounds", "[version_range]")
{
  CHECK(VersionRange("~1.2.3").contains(Version("1.2.9")));
  CHECK_FALSE(VersionRange("~1.2.3").contains(Version("1.3.0")));
  CHECK(VersionRange("^0.2.3").contains(Version("0.2.9")));
  CHECK_FALSE(VersionRange("^0.2.3").contains(Version("0.3.0")));
  CHECK(VersionRange("1.2 - 2.3").contains(Version("2.3.9")));
  CHECK_FALSE(VersionRange("1.2 - 2.3").contains(Version("2.4.0")));
}

TEST_CASE("VersionRange follows npm prerelease semantics", "[version_range]")
{
  CHECK(VersionRange(">=1.2.3-alpha <2.0.0").contains(Version("1.2.3-beta")));
  CHECK_FALSE(VersionRange(">=1.2.3 <2.0.0").contains(Version("1.2.4-alpha")));
  CHECK_FALSE(VersionRange("*").contains(Version("4.0.0-rc.1")));
  CHECK(VersionRange::any_with_prerelease().contains(Version("4.0.0-rc.1")));
  CHECK(VersionRange("1.2.3").contains(Version("1.2.3+build.5")));
}

TEST_CASE("VersionRange rejects invalid expressions", "[version_range]")
{
  for(auto const text : {
        std::string_view {"1.2.3 ||"},
        std::string_view {">="},
        std::string_view {"1.2.3 - *"},
        std::string_view {"1.2.3-alpha.01"},
        std::string_view {"1.02.x"},
        std::string_view {"1.x.3"},
        std::string_view {"1.2.3-"},
        std::string_view {"1.2.3+"},
      }) {
    CAPTURE(text);
    CHECK_THROWS_AS(VersionRange(text), VersionError);
  }
}

TEST_CASE("VersionRange supports normalization streams fmt and hash", "[version_range]")
{
  auto const range = VersionRange("  ^1.2.3  ");
  auto stream = std::ostringstream();
  stream << range;

  CHECK(range.to_string() == "^1.2.3");
  CHECK(stream.str() == "^1.2.3");
  CHECK(fmt::format("{}", range) == "^1.2.3");
  CHECK(std::unordered_set<VersionRange> {range, VersionRange("^1.2.3")}.size() == 1);
  CHECK(VersionRange().is_any());
  CHECK(VersionRange("").is_any());
  CHECK(VersionRange::any() != VersionRange::any_with_prerelease());
}
