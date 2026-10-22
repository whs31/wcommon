#include <radar/mms/common/check.h>

#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("Assertion macros throw and compose as statements", "[check]")
{
  REQUIRE_NOTHROW([] { RADAR_HARD_CHECK(true, std::runtime_error, "ok"); }());
  REQUIRE_THROWS_AS(
    [] { RADAR_HARD_CHECK(false, std::runtime_error, "failed"); }(),
    std::runtime_error
  );

  auto hard_check_else_branch = false;
  if(false)  // NOLINT(readability-simplify-boolean-expr): the macro must work as an unbraced if
             // body
    RADAR_HARD_CHECK(false, std::runtime_error, "unreachable");
  else
    hard_check_else_branch = true;
  REQUIRE(hard_check_else_branch);

  auto not_implemented = [] { RADAR_NOT_IMPLEMENTED };
  REQUIRE_THROWS_AS(not_implemented(), NotImplementedError);
}

TEST_CASE("Soft assertion macro follows the debug configuration", "[check]")
{
  auto evaluated = false;
  auto const evaluate = [&] {
    evaluated = true;
    return false;
  };
  auto check = [&] { RADAR_SOFT_CHECK(evaluate(), std::runtime_error, "failed"); };

#ifdef RADAR_DEBUG
  REQUIRE_THROWS_AS(check(), std::runtime_error);
  REQUIRE(evaluated);
#else
  REQUIRE_NOTHROW(check());
  REQUIRE_FALSE(evaluated);
#endif
}
