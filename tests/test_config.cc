#include <radar/mms/common/config.h>

#include <string_view>

#include <catch2/catch_test_macros.hpp>

#ifdef RCOMMON_WITH_QT
inline constexpr auto rcommon_with_qt = true;
#else
inline constexpr auto rcommon_with_qt = false;
#endif

TEST_CASE("Version macros encode and decode components", "[core][config]")
{
  constexpr auto version = RADAR_VERSION_ENCODE(12, 34, 56);

  STATIC_CHECK(version == 12'034'056);
  STATIC_CHECK(RADAR_VERSION_DECODE_MAJOR(version) == 12);
  STATIC_CHECK(RADAR_VERSION_DECODE_MINOR(version) == 34);
  STATIC_CHECK(RADAR_VERSION_DECODE_PATCH(version) == 56);
}

TEST_CASE("Preprocessor helpers expand arguments", "[core][config]")
{
#define RCOMMON_TEST_NUMBER 42
  constexpr auto RCOMMON_TEST_TOKEN_SUFFIX = 8;

  STATIC_CHECK(std::string_view {RADAR_STRINGIFY(RCOMMON_TEST_NUMBER)} == "42");
  STATIC_CHECK(RADAR_CONCAT(RCOMMON_TEST_TOKEN, _SUFFIX) == 8);
  STATIC_CHECK(RADAR_CONCAT3(RCOMMON_TEST_, TOKEN, _SUFFIX) == 8);
#undef RCOMMON_TEST_NUMBER
}

TEST_CASE("Generated configuration records Qt support", "[core][config][qt]")
{
#ifdef QT_CORE_LIB
  STATIC_CHECK(rcommon_with_qt);
#else
  STATIC_CHECK_FALSE(rcommon_with_qt);
#endif
}
