#include <radar/mms/common/graph_resolver.h>

RADAR_COMMON_NAMESPACE_BEGIN

void GraphResolver::enqueue(Uuid const& node, std::unordered_set<Uuid> const& leafs)
{
  this->tree()[node] = leafs;
}

std::vector<Uuid> GraphResolver::resolve(bool* has_cycle) const
{
  auto dependency_count = std::unordered_map<Uuid, std::size_t>();
  auto dependents = std::unordered_map<Uuid, std::vector<Uuid>>();
  auto ready = std::vector<Uuid>();
  auto result = std::vector<Uuid>();

  for(auto const& [node, leafs] : this->tree()) {
    dependency_count[node] = leafs.size();
    for(auto const& dependency : leafs) {
      dependency_count.try_emplace(dependency, 0);
      dependents[dependency].push_back(node);
    }
  }

  ready.reserve(dependency_count.size());
  result.reserve(dependency_count.size());
  for(auto const& [node, count] : dependency_count) {
    if(count == 0)
      ready.push_back(node);
  }

  while(not ready.empty()) {
    auto const node = ready.back();
    ready.pop_back();
    result.push_back(node);

    if(auto const it = dependents.find(node); it != dependents.end()) {
      for(auto const& dependent : it->second) {
        auto& count = dependency_count.at(dependent);
        --count;
        if(count == 0)
          ready.push_back(dependent);
      }
    }
  }

  if(has_cycle != nullptr)
    *has_cycle = result.size() != dependency_count.size();
  return result;
}

std::unordered_map<Uuid, std::unordered_set<Uuid>> const& GraphResolver::tree() const
{
  return this->m_tree;
}

std::unordered_map<Uuid, std::unordered_set<Uuid>>& GraphResolver::tree() { return this->m_tree; }

RADAR_COMMON_NAMESPACE_END
