#pragma once

#include <string>

#include <glaze/json.hpp>

#include <catch2/catch_test_macros.hpp>

// The helpers value-initialize the read target, which the read then overwrites. That is
// intended for enums without a zero enumerator, e.g. Qt::Key.
// NOLINTBEGIN(bugprone-invalid-enum-default-initialization)
namespace glaze_support
{
  template <typename T>
  [[nodiscard]] std::string write(T const& value)
  {
    auto result = glz::write_json(value);
    REQUIRE(result.has_value());
    return *result;
  }

  template <typename T>
  [[nodiscard]] T read(std::string const& json)
  {
    auto value = T {};
    auto const error = glz::read_json(value, json);
    INFO(glz::format_error(error, json));
    REQUIRE_FALSE(error);
    return value;
  }

  template <typename T>
  [[nodiscard]] glz::error_code read_error(std::string const& json)
  {
    auto value = T {};
    return glz::read_json(value, json).ec;
  }

  template <typename T>
  [[nodiscard]] glz::error_code strict_read_error(std::string const& json)
  {
    auto value = T {};
    return glz::read<glz::opts {.error_on_missing_keys = true}>(value, json).ec;
  }

  template <typename T>
  void check_round_trip(T const& value, std::string const& json)
  {
    REQUIRE(write(value) == json);
    REQUIRE(read<T>(json) == value);
  }
}  // namespace glaze_support

// NOLINTEND(bugprone-invalid-enum-default-initialization)
