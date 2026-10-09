#pragma once

#include <cstdint>
#include <array>

#include <nlohmann/json.hpp>

#include <radar/mms/common/error.h>
#include <radar/mms/common/random.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Serializes the generator state as `{"state": [s0, s1, s2, s3]}`, e.g. for snapshots.
 * @ingroup core_random
 */
inline void to_json(nlohmann::json& json, RandomStream const& stream)
{
  json = {{"state", stream.state}};
}

/**
 * @brief Restores a generator state.
 * @throws InvalidArgumentError if the state is all zero, which the generator cannot leave.
 */
inline void from_json(nlohmann::json const& json, RandomStream& stream)
{
  auto const state = json.at("state").get<std::array<std::uint64_t, 4>>();
  if(state == std::array<std::uint64_t, 4> {})
    throw InvalidArgumentError("invalid random stream conversion from json: all-zero state");
  stream = RandomStream {.state = state};
}

RADAR_COMMON_NAMESPACE_END
