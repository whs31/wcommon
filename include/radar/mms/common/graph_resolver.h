#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/uuid.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Resolves UUID-keyed dependency graphs into dependency-first order.
 * @details GraphResolver stores directed edges from a node to the nodes it
 * depends on. Calling resolve() performs an iterative topological traversal
 * and returns every known UUID exactly once, with each dependency placed before
 * the node that references it when no cycle blocks that part of the graph.
 *
 * The graph is keyed by Uuid because it is intended for systems where objects
 * are already identified by stable IDs. Dependencies may be enqueued either as
 * explicit nodes or only as leaf entries; resolve() still includes leaf-only
 * UUIDs in the output.
 *
 * @code {.cpp}
 * auto resolver = radar::mms::common::GraphResolver();
 *
 * auto const database = radar::mms::common::Uuid("00000000-0000-4000-8000-000000000001");
 * auto const cache = radar::mms::common::Uuid("00000000-0000-4000-8000-000000000002");
 * auto const service = radar::mms::common::Uuid("00000000-0000-4000-8000-000000000003");
 *
 * resolver.enqueue(service, {database, cache});
 * resolver.enqueue(cache, {database});
 *
 * auto has_cycle = false;
 * auto const order = resolver.resolve(&has_cycle);
 * // has_cycle == false
 * // database appears before cache and service; cache appears before service.
 * @endcode
 *
 * @code {.cpp}
 * auto resolver = radar::mms::common::GraphResolver();
 * auto const a = radar::mms::common::Uuid("00000000-0000-4000-8000-000000000001");
 * auto const b = radar::mms::common::Uuid("00000000-0000-4000-8000-000000000002");
 *
 * resolver.enqueue(a, {b});
 * resolver.enqueue(b, {a});
 *
 * auto has_cycle = false;
 * auto const partial_order = resolver.resolve(&has_cycle);
 * // has_cycle == true; partial_order contains nodes that are not blocked by
 * // the cycle.
 * @endcode
 *
 * @warning Iteration order for unordered containers is not stable. Treat the
 * returned vector as a valid topological order, not as a deterministic ordering
 * between unrelated nodes.
 *
 * @ingroup core
 */
class RADAR_COMMON_API GraphResolver final
{
 public:
  /**
   * @brief Creates an empty resolver.
   */
  GraphResolver() = default;

  /**
   * @brief Copying is disabled because the resolver owns its mutable graph.
   */
  GraphResolver(GraphResolver const&) = delete;

  /**
   * @brief Copy assignment is disabled because the resolver owns its mutable graph.
   */
  GraphResolver& operator=(GraphResolver const&) = delete;

  /**
   * @brief Move-constructs a resolver and transfers the stored graph.
   */
  GraphResolver(GraphResolver&&) = default;

  /**
   * @brief Move-assigns a resolver and transfers the stored graph.
   */
  GraphResolver& operator=(GraphResolver&&) = default;

  /**
   * @brief Destroys the resolver and its stored graph.
   */
  ~GraphResolver() = default;

  /**
   * @brief Adds or replaces dependencies for @p node.
   * @details The stored edge direction is `node -> leaf`, where every UUID in
   * @p leafs must be resolved before @p node. Calling enqueue() again for the
   * same @p node replaces the previous dependency set.
   *
   * @param node Node whose dependencies are being registered.
   * @param leafs Dependency UUIDs that must appear before @p node in an
   * acyclic result.
   */
  void enqueue(Uuid const& node, std::unordered_set<Uuid> const& leafs);

  /**
   * @brief Resolves the stored graph into dependency-first order.
   * @details The result contains every UUID that appears either as an enqueued
   * node or as a dependency leaf. For acyclic graphs, each dependency appears
   * before every node that depends on it. For cyclic graphs, nodes outside the
   * cycle are still resolved when they do not depend on it; the returned vector
   * is therefore a partial order rather than a complete topological sort.
   *
   * @param has_cycle Optional output flag. When non-null, it is set to `false`
   * before traversal and set to `true` if any cycle is detected.
   * @return Dependency-first resolution order.
   */
  [[nodiscard]] std::vector<Uuid> resolve(bool* has_cycle = nullptr) const;

  /**
   * @brief Returns the stored dependency map.
   */
  [[nodiscard]] std::unordered_map<Uuid, std::unordered_set<Uuid>> const& tree() const;

  /**
   * @brief Returns the mutable stored dependency map.
   * @details This allows bulk edits by callers that already have a map-like
   * representation. Mutating the map directly has the same semantics as
   * enqueue(): keys are nodes and values are their dependency leaves.
   */
  [[nodiscard]] std::unordered_map<Uuid, std::unordered_set<Uuid>>& tree();

 private:
  std::unordered_map<Uuid, std::unordered_set<Uuid>> m_tree;
};

RADAR_COMMON_NAMESPACE_END
