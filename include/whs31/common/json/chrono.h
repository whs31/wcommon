#pragma once

#include <chrono>
#include <cstdint>

#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <typename Clock, typename Duration>
struct adl_serializer<std::chrono::time_point<Clock, Duration>>
{
  static void to_json(json& value, std::chrono::time_point<Clock, Duration> const& time_point)
  {
    value = {
      {
        "since_epoch",
        std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch())
          .count(),
      },
      {"unit", "milliseconds"},
    };
  }

  static void from_json(json const& value, std::chrono::time_point<Clock, Duration>& time_point)
  {
    auto const milliseconds = std::chrono::milliseconds(
      value.at("since_epoch").get<std::int64_t>()
    );
    time_point = std::chrono::time_point<Clock, Duration>(
      std::chrono::duration_cast<Duration>(milliseconds)
    );
  }
};

template <typename Rep, typename Ratio>
struct adl_serializer<std::chrono::duration<Rep, Ratio>>
{
  static void to_json(json& value, std::chrono::duration<Rep, Ratio> const& duration)
  {
    value = duration.count();
  }

  static void from_json(json const& value, std::chrono::duration<Rep, Ratio>& duration)
  {
    duration = std::chrono::duration<Rep, Ratio>(value.template get<Rep>());
  }
};

NLOHMANN_JSON_NAMESPACE_END
