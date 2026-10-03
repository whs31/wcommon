#include <radar/mms/common/math.h>
#include <radar/mms/common/point2.h>
#include <radar/mms/common/point3.h>
#include <radar/mms/common/vector2.h>
#include <radar/mms/common/vector3.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <unordered_set>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms::common;

TEST_CASE("Geometry values support constexpr vector math", "[geometry]")
{
  STATIC_CHECK(Vector2::from_xy(3.0, 4.0).length_squared() == 25.0);
  STATIC_CHECK(Vector2::from_xy(1.0, 0.0).cross(Vector2::from_xy(0.0, 1.0)) == 1.0);
  STATIC_CHECK(
    Vector3::from_xyz(1.0, 0.0, 0.0).cross(Vector3::from_xyz(0.0, 1.0, 0.0))
    == Vector3::from_xyz(0.0, 0.0, 1.0)
  );
  STATIC_CHECK(Point2::from_xy(1.0, 2.0).to_vector2() == Vector2::from_xy(1.0, 2.0));
  STATIC_CHECK(
    Point3::from_vector3(Vector3::from_xyz(1.0, 2.0, 3.0)).to_vector3()
    == Vector3::from_xyz(1.0, 2.0, 3.0)
  );
}

TEST_CASE("Points translate and measure", "[geometry]")
{
  auto const point = Point2::from_xy(1.0, 2.0);
  auto const moved = point + Vector2::from_xy(3.0, 4.0);
  REQUIRE(moved == Point2::from_xy(4.0, 6.0));
  REQUIRE(moved - point == Vector2::from_xy(3.0, 4.0));
  REQUIRE(approx_eq(point.distance_to(moved), 5.0));
  REQUIRE(approx_eq(point.angle_to(Point2::from_xy(1.0, 3.0)).degrees(), 90.0, 1e-4));

  auto const point3 = Point3::from_xyz(1.0, 2.0, 3.0);
  REQUIRE(point3.xy() == Point2::from_xy(1.0, 2.0));
  REQUIRE(point3 + Vector3::from_xyz(4.0, 5.0, 6.0) == Point3::from_xyz(5.0, 7.0, 9.0));
}

TEST_CASE("Vectors normalize, rotate and handle a zero length", "[geometry]")
{
  REQUIRE(approx_eq(Vector2::from_xy(3.0, 4.0).normalized().length(), 1.0));
  REQUIRE(approx_eq(Vector3::from_xyz(0.0, 3.0, 4.0).normalized().length(), 1.0));
  REQUIRE(Vector2::zero().normalized() == Vector2::zero());
  REQUIRE(Vector3::zero().normalized() == Vector3::zero());
  REQUIRE(Vector2::from_xy(1.0, 0.0).rotated(Angle::frac_pi_2()).y() > 0.999);
  REQUIRE_FALSE(
    std::isfinite(Vector2::from_xy(std::numeric_limits<double>::infinity(), 0.0).length())
  );
}

TEST_CASE("Geometry values format, stream and hash", "[geometry]")
{
  REQUIRE(Vector2::from_xy(1.0, 2.0).to_string() == "(1, 2)");
  REQUIRE(fmt::format("{}", Vector3::from_xyz(1.0, 2.0, 3.0)) == "(1, 2, 3)");

  auto stream = std::ostringstream();
  stream << Point3::from_xyz(1.0, 2.0, 3.0);
  REQUIRE(stream.str() == "(1, 2, 3)");

  auto values = std::unordered_set<Point2> {Point2::from_xy(1.0, 2.0)};
  REQUIRE(values.contains(Point2::from_xy(1.0, 2.0)));
}
