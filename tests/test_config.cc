#include <whs31/common/config.h>

#include <string_view>

#include <catch2/catch_test_macros.hpp>

#ifdef WHS31_COMMON_WITH_QT
inline auto constexpr wcommon_with_qt = true;
#else
inline auto constexpr wcommon_with_qt = false;
#endif

TEST_CASE("Version macros encode and decode components", "[core][config]")
{
  auto constexpr version = WHS31_VERSION_ENCODE(12, 34, 56);

  STATIC_CHECK(version == 12'034'056);
  STATIC_CHECK(WHS31_VERSION_DECODE_MAJOR(version) == 12);
  STATIC_CHECK(WHS31_VERSION_DECODE_MINOR(version) == 34);
  STATIC_CHECK(WHS31_VERSION_DECODE_PATCH(version) == 56);
}

TEST_CASE("Preprocessor helpers expand arguments", "[core][config]")
{
#define WCOMMON_TEST_NUMBER 42  // NOLINT(cppcoreguidelines-macro-usage): the helpers expand macros
  auto constexpr WCOMMON_TEST_TOKEN_SUFFIX = 8;

  STATIC_CHECK(std::string_view {WHS31_STRINGIFY(WCOMMON_TEST_NUMBER)} == "42");
  STATIC_CHECK(WHS31_CONCAT(WCOMMON_TEST_TOKEN, _SUFFIX) == 8);
  STATIC_CHECK(WHS31_CONCAT3(WCOMMON_TEST_, TOKEN, _SUFFIX) == 8);
#undef WCOMMON_TEST_NUMBER
}

TEST_CASE("Generated configuration records Qt support", "[core][config][qt]")
{
#ifdef QT_CORE_LIB
  STATIC_CHECK(wcommon_with_qt);
#else
  STATIC_CHECK_FALSE(wcommon_with_qt);
#endif
}
