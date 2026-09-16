#include <radar/mms/common/functional.h>

#include <string>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

namespace
{
  struct ThrowingMoveCallable
  {
    ThrowingMoveCallable() = default;

    ThrowingMoveCallable(ThrowingMoveCallable&&) noexcept(false) {}

    int operator()() const { return 42; }
  };

  struct ThrowingCallback
  {
    void operator()() const {}
  };

  struct ReturningCallback
  {
    int operator()() const noexcept { return 42; }
  };

  struct NotCallable
  {};

  struct LvalueCallback
  {
    void operator()() & noexcept {}
  };

  struct RvalueCallback
  {
    void operator()() && noexcept {}
  };

  template <typename Callback>
  concept ScopeGuardCallback = requires(Callback&& callback) {
    make_scope_guard(std::forward<Callback>(callback));
  };

  int overloaded_function(int const value) { return value + 10; }

  double overloaded_function(double const value) { return value + 0.5; }

  struct OverloadedMember
  {
    int value(int const input) { return input + 1; }

    int value(int const input) const { return input + 2; }

    std::string value(char const* input) const { return input; }
  };
}  // namespace

TEST_CASE("Overload has proper type traits", "[core][functional]")
{
  STATIC_CHECK(std::is_invocable_v<decltype(overload<int>), int (*)(int)>);
  STATIC_CHECK(std::is_invocable_v<decltype(overload<double>), double (*)(double)>);
}

TEST_CASE("Lazy has proper type traits", "[core][functional]")
{
  auto value = lazy(ThrowingMoveCallable {});

  STATIC_CHECK_FALSE(std::is_nothrow_move_constructible_v<decltype(value)>);
  STATIC_CHECK_FALSE(std::is_copy_constructible_v<decltype(value)>);
  STATIC_CHECK_FALSE(std::is_copy_assignable_v<decltype(value)>);
  STATIC_CHECK_FALSE(std::is_move_assignable_v<decltype(value)>);
}

TEST_CASE("Lazy evaluates once and returns the cached value", "[core][functional]")
{
  auto evaluations = 0;
  auto value = lazy([&] {
    ++evaluations;
    return std::string {"computed"};
  });

  REQUIRE(evaluations == 0);
  REQUIRE(value() == "computed");
  REQUIRE(evaluations == 1);
  REQUIRE(value() == "computed");
  REQUIRE(evaluations == 1);

  value() = "cached";
  REQUIRE(value() == "cached");
  REQUIRE(evaluations == 1);
}

TEST_CASE("Lazy moves cached values without re-evaluating", "[core][functional]")
{
  auto evaluations = 0;
  auto value = lazy([&] {
    ++evaluations;
    return std::string {"first"};
  });

  REQUIRE(value() == "first");

  auto moved = std::move(value);
  REQUIRE(moved() == "first");
  REQUIRE(evaluations == 1);
}

TEST_CASE("Lazy evaluates through a const wrapper", "[core][functional]")
{
  auto evaluations = 0;
  auto const value = lazy([&] {
    ++evaluations;
    return 42;
  });

  STATIC_CHECK(std::is_same_v<decltype(value()), int const&>);
  REQUIRE(value() == 42);
  REQUIRE(value() == 42);
  REQUIRE(evaluations == 1);
}

TEST_CASE("Lazy moves unevaluated values", "[core][functional]")
{
  auto evaluations = 0;
  auto value = lazy([&] {
    ++evaluations;
    return 42;
  });

  auto moved = std::move(value);

  REQUIRE(evaluations == 0);
  REQUIRE(moved() == 42);
  REQUIRE(evaluations == 1);
}

TEST_CASE("Lazy retries evaluation after an exception", "[core][functional]")
{
  auto evaluations = 0;
  auto value = lazy([&] {
    if(++evaluations == 1)
      throw std::runtime_error {"first evaluation failed"};

    return 42;
  });

  REQUIRE_THROWS_AS(value(), std::runtime_error);
  REQUIRE(value() == 42);
  REQUIRE(evaluations == 2);
}

TEST_CASE("Overload helpers select free and member function overloads", "[core][functional]")
{
  auto const int_function = overload<int>(overloaded_function);
  auto const double_function = overload<double>(overloaded_function);

  REQUIRE(int_function(5) == 15);
  REQUIRE(double_function(2.0) == 2.5);

  auto object = OverloadedMember();
  auto const const_object = OverloadedMember();

  auto const mutable_member = non_const_overload<int>(&OverloadedMember::value);
  auto const const_member = const_overload<int>(&OverloadedMember::value);
  auto const string_member = overload<char const*>(&OverloadedMember::value);

  REQUIRE((object.*mutable_member)(3) == 4);
  REQUIRE((const_object.*const_member)(3) == 5);
  REQUIRE((const_object.*string_member)("text") == "text");
}

TEST_CASE("Overload static helpers select overloads", "[core][functional]")
{
  auto const free_function = overload_t<int>::of(overloaded_function);
  auto const mutable_member = non_const_overload_t<int>::of(&OverloadedMember::value);
  auto const const_member = const_overload_t<int>::of(&OverloadedMember::value);
  auto object = OverloadedMember();
  auto const const_object = OverloadedMember();

  REQUIRE(free_function(5) == 15);
  REQUIRE((object.*mutable_member)(3) == 4);
  REQUIRE((const_object.*const_member)(3) == 5);
}

TEST_CASE("Scope guards run on scope exit unless dismissed", "[core][functional]")
{
  auto value = 0;

  {
    auto const guard = defer([&]() noexcept { value = 1; });
    static_cast<void>(guard);
    REQUIRE(value == 0);
  }
  REQUIRE(value == 1);

  {
    auto guard = make_scope_guard([&]() noexcept { value = 2; });
    guard.dismiss();
  }
  REQUIRE(value == 1);
}

TEST_CASE("Scope guards accept only noexcept void callbacks", "[core][functional]")
{
  STATIC_CHECK(ScopeGuardCallback<decltype([]() noexcept {})>);
  STATIC_CHECK_FALSE(ScopeGuardCallback<ThrowingCallback>);
  STATIC_CHECK_FALSE(ScopeGuardCallback<ReturningCallback>);
  STATIC_CHECK_FALSE(ScopeGuardCallback<NotCallable>);
  STATIC_CHECK(ScopeGuardCallback<LvalueCallback>);
  STATIC_CHECK_FALSE(ScopeGuardCallback<RvalueCallback>);
}

TEST_CASE("Scope guard move transfers cleanup ownership", "[core][functional]")
{
  auto value = 0;

  {
    auto guard = make_scope_guard([&]() noexcept { ++value; });
    auto const moved = std::move(guard);
    static_cast<void>(moved);
  }

  REQUIRE(value == 1);
}
