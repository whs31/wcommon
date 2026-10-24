#include <whs31/common/json/point2.h>
#include <whs31/common/json/point3.h>
#include <whs31/common/json/vector2.h>
#include <whs31/common/json/vector3.h>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace whs31::common;

TEST_CASE("Geometry JSON adapters round-trip their coordinate objects", "[geometry][json]")
{
  auto constexpr point2 = Point2::from_xy(4.5, 5.5);
  auto constexpr point3 = Point3::from_xyz(4.5, 5.5, 6.5);
  auto constexpr vector2 = Vector2::from_xy(1.5, 2.5);
  auto constexpr vector3 = Vector3::from_xyz(1.5, 2.5, 3.5);

  REQUIRE(nlohmann::json(point2) == nlohmann::json({{"x", 4.5}, {"y", 5.5}}));
  REQUIRE(nlohmann::json(point3) == nlohmann::json({{"x", 4.5}, {"y", 5.5}, {"z", 6.5}}));
  REQUIRE(nlohmann::json(vector2).get<Vector2>() == vector2);
  REQUIRE(nlohmann::json(vector3).get<Vector3>() == vector3);
}

TEST_CASE(
  "Geometry JSON adapters reject missing and incorrectly typed coordinates",
  "[geometry][json]"
)
{
  REQUIRE_THROWS(nlohmann::json({{"x", 1.0}}).get<Point2>());
  REQUIRE_THROWS(nlohmann::json({{"x", 1.0}, {"y", 2.0}}).get<Point3>());
  REQUIRE_THROWS(nlohmann::json({{"x", "1"}, {"y", 2.0}}).get<Vector2>());
  REQUIRE_THROWS(nlohmann::json({{"x", 1.0}, {"y", 2.0}, {"z", nullptr}}).get<Vector3>());
}
