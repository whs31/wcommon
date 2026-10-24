#include <whs31/common/adapter.h>

#include <type_traits>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

namespace
{
  struct Widget
  {
    int value = 0;

    void set_value(int const new_value) noexcept { this->value = new_value; }
  };

  class WidgetAdapter final : public Adapter<Widget>
  {
   public:
    using Adapter::Adapter;

    void set_value(int const value) { this->adaptee().set_value(value); }

    [[nodiscard]] int value() const { return this->adaptee().value; }
  };

  class ConstWidgetAdapter final : public Adapter<Widget const>
  {
   public:
    using Adapter::Adapter;

    [[nodiscard]] int value() const { return this->adaptee().value; }
  };
}  // namespace

TEST_CASE("Adapter preserves adaptee constness", "[adapter]")
{
  STATIC_CHECK(std::is_constructible_v<WidgetAdapter, Widget&>);
  STATIC_CHECK(not std::is_constructible_v<WidgetAdapter, Widget const&>);
  STATIC_CHECK(std::is_constructible_v<ConstWidgetAdapter, Widget const&>);
  STATIC_CHECK(std::is_constructible_v<ConstWidgetAdapter, Widget&>);
}

TEST_CASE("Adapter observes and mutates a referenced adaptee", "[adapter]")
{
  auto widget = Widget {1};
  auto adapter = WidgetAdapter(widget);

  REQUIRE(adapter.has_adaptee());
  REQUIRE(adapter.adaptee_ptr().get() == &widget);
  REQUIRE(adapter.value() == 1);

  adapter.set_value(7);
  REQUIRE(widget.value == 7);
}

TEST_CASE("Adapter accepts observer_ptr", "[adapter]")
{
  auto widget = Widget {2};
  auto adapter = WidgetAdapter(make_observer(&widget));

  REQUIRE(adapter.has_adaptee());
  REQUIRE(&adapter.adaptee() == &widget);

  adapter.set_value(9);
  REQUIRE(widget.value == 9);
}

TEST_CASE("Adapter supports read-only adaptees", "[adapter]")
{
  auto const widget = Widget {3};
  auto adapter = ConstWidgetAdapter(widget);

  REQUIRE(adapter.has_adaptee());
  REQUIRE(adapter.adaptee_ptr().get() == &widget);
  REQUIRE(adapter.value() == 3);
}

TEST_CASE("Adapter exposes an empty observer without dereferencing it", "[adapter]")
{
  auto adapter = WidgetAdapter(observer_ptr<Widget> {});

  REQUIRE_FALSE(adapter.has_adaptee());
  REQUIRE(adapter.adaptee_ptr() == nullptr);
}

TEST_CASE("Adapter promote restores mutability of an originally mutable object", "[adapter]")
{
  auto widget = Widget {4};
  auto const* const_view = &widget;

  auto* mutable_view = WidgetAdapter::promote(const_view);
  mutable_view->set_value(5);

  REQUIRE(mutable_view == &widget);
  REQUIRE(widget.value == 5);
}
