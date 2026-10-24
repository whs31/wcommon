#include <whs31/common/graph_resolver.h>

#include <cstddef>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <catch2/catch_test_macros.hpp>

using namespace whs31::common;

static_assert(std::is_default_constructible_v<GraphResolver>);
static_assert(not std::is_copy_constructible_v<GraphResolver>);
static_assert(std::is_move_constructible_v<GraphResolver>);

namespace
{
  [[nodiscard]] Uuid id(int const value)
  {
    return Uuid(fmt::format("00000000-0000-4000-8000-{:012x}", value));
  }

  [[nodiscard]] std::unordered_map<Uuid, std::size_t> positions(std::vector<Uuid> const& nodes)
  {
    auto result = std::unordered_map<Uuid, std::size_t>();
    for(auto i = std::size_t {0}; i < nodes.size(); ++i)
      result.emplace(nodes.at(i), i);
    return result;
  }
}  // namespace

TEST_CASE("GraphResolver replaces dependency sets and exposes the tree", "[graph_resolver]")
{
  auto resolver = GraphResolver();
  resolver.enqueue(id(1), {id(2)});
  resolver.enqueue(id(1), {id(3)});
  REQUIRE(resolver.tree().at(id(1)) == std::unordered_set<Uuid> {id(3)});

  resolver.tree()[id(4)].insert(id(5));
  REQUIRE(resolver.tree().at(id(4)).contains(id(5)));
}

TEST_CASE(
  "GraphResolver produces dependency-first order including leaf-only nodes",
  "[graph_resolver]"
)
{
  auto resolver = GraphResolver();
  resolver.enqueue(id(2), {id(1)});
  resolver.enqueue(id(3), {id(1), id(2)});
  resolver.enqueue(id(4), {});

  auto has_cycle = true;
  auto const order = resolver.resolve(&has_cycle);
  auto const pos = positions(order);

  REQUIRE_FALSE(has_cycle);
  REQUIRE(order.size() == 4);
  REQUIRE(pos.at(id(1)) < pos.at(id(2)));
  REQUIRE(pos.at(id(2)) < pos.at(id(3)));
  REQUIRE(pos.contains(id(4)));
}

TEST_CASE("GraphResolver resolves empty and disconnected graphs", "[graph_resolver]")
{
  auto resolver = GraphResolver();
  REQUIRE(resolver.resolve().empty());

  resolver.enqueue(id(1), {});
  resolver.enqueue(id(2), {});
  auto const order = resolver.resolve();
  REQUIRE(order.size() == 2);
  REQUIRE(positions(order).contains(id(1)));
  REQUIRE(positions(order).contains(id(2)));
}

TEST_CASE("GraphResolver detects self and multi-node cycles", "[graph_resolver]")
{
  SECTION("self cycle")
  {
    auto resolver = GraphResolver();
    resolver.enqueue(id(1), {id(1)});
    auto has_cycle = false;
    static_cast<void>(resolver.resolve(&has_cycle));
    REQUIRE(has_cycle);
  }

  SECTION("multi-node cycle")
  {
    auto resolver = GraphResolver();
    resolver.enqueue(id(1), {id(2)});
    resolver.enqueue(id(2), {id(3)});
    resolver.enqueue(id(3), {id(1)});
    auto has_cycle = false;
    static_cast<void>(resolver.resolve(&has_cycle));
    REQUIRE(has_cycle);
  }
}

TEST_CASE("GraphResolver handles long dependency chains without recursion", "[graph_resolver]")
{
  auto constexpr node_count = 20'000;
  auto resolver = GraphResolver();
  resolver.enqueue(id(0), {});
  for(auto value = 1; value < node_count; ++value)
    resolver.enqueue(id(value), {id(value - 1)});

  auto has_cycle = true;
  auto const order = resolver.resolve(&has_cycle);
  auto const pos = positions(order);

  REQUIRE_FALSE(has_cycle);
  REQUIRE(order.size() == node_count);
  REQUIRE(pos.at(id(0)) < pos.at(id(node_count - 1)));
}

TEST_CASE("GraphResolver keeps independent nodes when another component cycles", "[graph_resolver]")
{
  auto resolver = GraphResolver();
  resolver.enqueue(id(1), {id(2)});
  resolver.enqueue(id(2), {id(1)});
  resolver.enqueue(id(3), {});

  auto has_cycle = false;
  auto const order = resolver.resolve(&has_cycle);

  REQUIRE(has_cycle);
  REQUIRE(positions(order).contains(id(3)));
}
