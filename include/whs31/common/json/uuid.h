#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <whs31/common/uuid.h>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<whs31::common::Uuid>
{
  static void to_json(json& value, whs31::common::Uuid const& uuid) { value = uuid.to_string(); }

  static void from_json(json const& value, whs31::common::Uuid& uuid)
  {
    uuid = whs31::common::Uuid(value.get<std::string>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
