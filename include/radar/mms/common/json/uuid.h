#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <radar/mms/common/uuid.h>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<radar::mms::common::Uuid>
{
  static void to_json(json& value, radar::mms::common::Uuid const& uuid)
  {
    value = uuid.to_string();
  }

  static void from_json(json const& value, radar::mms::common::Uuid& uuid)
  {
    uuid = radar::mms::common::Uuid(value.get<std::string>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
