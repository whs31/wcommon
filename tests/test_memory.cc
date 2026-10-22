#include <radar/mms/common/memory.h>
#include <radar/mms/common/pimpl.h>

#include <array>
#include <memory>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

namespace
{
  struct Base
  {
    int value = 0;
  };

  struct Derived : Base
  {};

  struct Widget
  {
    int value = 0;

    [[nodiscard]] int get() const noexcept { return this->value; }

    void set(int const new_value) noexcept { this->value = new_value; }
  };

  [[nodiscard]] int& deleted_value() noexcept
  {
    static auto value = 0;
    return value;
  }

  int delete_int(int const* const value) noexcept
  {
    deleted_value() = *value;
    delete value;  // NOLINT(cppcoreguidelines-owning-memory): a raw-pointer deleter is what the
                   // test exercises
    return 0;
  }

  struct UniqueOwner
  {
    DECLARE_PRIVATE(UniqueOwner)

    explicit UniqueOwner(int value);

    [[nodiscard]] int value() const noexcept;
    void set_value(int value) noexcept;
  };

  struct UniqueOwner::UniqueOwnerPrivate : Widget
  {};

  UniqueOwner::UniqueOwner(int const value)
    : d(std::make_unique<UniqueOwnerPrivate>(UniqueOwnerPrivate {value}))
  {}

  int UniqueOwner::value() const noexcept { return this->d->get(); }

  void UniqueOwner::set_value(int const value) noexcept { this->d->set(value); }

  struct SharedOwner
  {
    DECLARE_PRIVATE_SHARED(SharedOwner)

    explicit SharedOwner(std::shared_ptr<SharedOwnerPrivate> impl);

    [[nodiscard]] int value() const noexcept;
    void set_value(int value) noexcept;
  };

  struct SharedOwner::SharedOwnerPrivate : Widget
  {};

  SharedOwner::SharedOwner(std::shared_ptr<SharedOwnerPrivate> impl)
    : d(std::move(impl))
  {}

  int SharedOwner::value() const noexcept { return this->d->get(); }

  void SharedOwner::set_value(int const value) noexcept { this->d->set(value); }

  template <typename To, typename From>
  concept CanObserverCast = requires(From& from) { observer_cast<To>(from); };
}  // namespace

TEST_CASE("Memory classes have the intended type traits", "[memory]")
{
  STATIC_CHECK(std::is_same_v<Owner<int*>, int*>);
  STATIC_CHECK(std::is_same_v<decltype(std::declval<Pimpl<Widget> const&>().get()), Widget const*>);
  STATIC_CHECK(
    std::is_same_v<decltype(std::declval<SharedPimpl<Widget> const&>().get()), Widget const*>
  );
  STATIC_CHECK(CanObserverCast<int, int>);
  STATIC_CHECK(CanObserverCast<int, int*>);
  STATIC_CHECK(CanObserverCast<int, std::unique_ptr<int>>);
  STATIC_CHECK(CanObserverCast<int const, int const>);
  STATIC_CHECK_FALSE(CanObserverCast<int, int const>);
}

TEST_CASE("observer_ptr observes, resets, releases and swaps raw pointers", "[memory]")
{
  auto values = std::array {Base {1}, Base {2}};

  auto first = observer_ptr<Base> {values.data()};
  auto second = make_observer(&values.at(1));
  auto empty = observer_ptr<Base> {};

  REQUIRE(first);
  REQUIRE_FALSE(empty);
  REQUIRE(first != nullptr);
  REQUIRE(nullptr != first);
  REQUIRE(empty == nullptr);
  REQUIRE(nullptr == empty);
  REQUIRE(first.get() == values.data());
  REQUIRE(first->value == 1);
  REQUIRE(first.ref().value == 1);
  REQUIRE(first != second);
  REQUIRE(first < second);
  REQUIRE(first <= second);
  REQUIRE(second > first);
  REQUIRE(second >= first);

  first.swap(second);
  REQUIRE(first.get() == &values.at(1));
  REQUIRE(second.get() == values.data());

  auto* released = first.release();
  REQUIRE(released == &values.at(1));
  REQUIRE_FALSE(first);

  first.reset(values.data());
  REQUIRE(first == second);
}

TEST_CASE("observer_ptr converts from compatible observers and owners", "[memory]")
{
  auto derived = Derived {};
  auto const derived_observer = observer_ptr<Derived> {&derived};
  auto const base_observer = observer_ptr<Base> {derived_observer};
  REQUIRE(base_observer.get() == &derived);

  auto const owned = std::make_unique<Derived>();
  auto const owning_observer = observer_ptr<Base> {owned};
  REQUIRE(owning_observer.get() == owned.get());
}

TEST_CASE("observer_ptr ordering follows the standard pointer total order", "[memory]")
{
  auto first_owner = std::make_unique<int>(1);
  auto second_owner = std::make_unique<int>(2);
  auto const first = make_observer(first_owner.get());
  auto const second = make_observer(second_owner.get());
  auto const less = std::less<> {}(first.get(), second.get());

  REQUIRE((first < second) == less);
  REQUIRE((first > second) == not less);
  REQUIRE((first <= second) == less);
  REQUIRE((first >= second) == not less);
}

TEST_CASE("observer_ptr reports null dereferences in debug builds", "[memory]")
{
#ifdef RADAR_DEBUG
  auto empty = observer_ptr<int> {};
  auto const const_empty = observer_ptr<int> {};

  REQUIRE_THROWS_AS(empty.ref(), NullPointerError);
  REQUIRE_THROWS_AS(const_empty.ref(), NullPointerError);
  REQUIRE_THROWS_AS(*empty, NullPointerError);
  REQUIRE_THROWS_AS(empty.operator->(), NullPointerError);
#else
  SUCCEED("Null checks are intentionally disabled in release builds");
#endif
}

TEST_CASE("observer_cast accepts object and pointer-like lvalues", "[memory]")
{
  auto value = 42;
  auto* pointer = &value;
  auto owner = std::make_unique<int>(17);
  auto const const_value = 23;

  REQUIRE(observer_cast<int>(value).get() == &value);
  REQUIRE(observer_cast<int>(pointer).get() == &value);
  REQUIRE(observer_cast<int>(owner).get() == owner.get());
  REQUIRE(observer_cast<int const>(const_value).get() == &const_value);
}

TEST_CASE("Deleter adapts C-style cleanup functions for unique_ptr", "[memory]")
{
  using IntDeleter = Deleter<int, int const*, delete_int>;

  deleted_value() = 0;
  {
    auto const value = std::unique_ptr<int, IntDeleter> {new int {42}};
    REQUIRE(*value == 42);
  }

  REQUIRE(deleted_value() == 42);
}

TEST_CASE("Pimpl wrappers propagate constness to their implementations", "[pimpl]")
{
  auto unique_owner = UniqueOwner {7};
  auto const& const_unique_owner = unique_owner;

  REQUIRE(const_unique_owner.value() == 7);
  unique_owner.set_value(11);
  REQUIRE(const_unique_owner.value() == 11);

  auto shared_impl = std::make_shared<SharedOwner::SharedOwnerPrivate>(
    SharedOwner::SharedOwnerPrivate {3}
  );
  auto shared_owner = SharedOwner {shared_impl};
  auto const& const_shared_owner = shared_owner;

  REQUIRE(const_shared_owner.value() == 3);
  shared_owner.set_value(13);
  REQUIRE(shared_impl->value == 13);
  REQUIRE(const_shared_owner.value() == 13);
}
