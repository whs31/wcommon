#include <radar/mms/common/json/point2.h>
#include <radar/mms/common/json/point3.h>
#include <radar/mms/common/json/vector2.h>
#include <radar/mms/common/json/vector3.h>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms::common;

TEST_CASE("Geometry JSON adapters round-trip their coordinate objects", "[geometry][json]")
{
  constexpr auto point2 = Point2::from_xy(4.5F, 5.5F);
  constexpr auto point3 = Point3::from_xyz(4.5F, 5.5F, 6.5F);
  constexpr auto vector2 = Vector2::from_xy(1.5F, 2.5F);
  constexpr auto vector3 = Vector3::from_xyz(1.5F, 2.5F, 3.5F);

  REQUIRE(nlohmann::json(point2) == nlohmann::json({{"x", 4.5F}, {"y", 5.5F}}));
  REQUIRE(nlohmann::json(point3) == nlohmann::json({{"x", 4.5F}, {"y", 5.5F}, {"z", 6.5F}}));
  REQUIRE(nlohmann::json(vector2).get<Vector2>() == vector2);
  REQUIRE(nlohmann::json(vector3).get<Vector3>() == vector3);
}

TEST_CASE(
  "Geometry JSON adapters reject missing and incorrectly typed coordinates",
  "[geometry][json]"
)
{
  REQUIRE_THROWS(nlohmann::json({{"x", 1.0F}}).get<Point2>());
  REQUIRE_THROWS(nlohmann::json({{"x", 1.0F}, {"y", 2.0F}}).get<Point3>());
  REQUIRE_THROWS(nlohmann::json({{"x", "1"}, {"y", 2.0F}}).get<Vector2>());
  REQUIRE_THROWS(nlohmann::json({{"x", 1.0F}, {"y", 2.0F}, {"z", nullptr}}).get<Vector3>());
}
