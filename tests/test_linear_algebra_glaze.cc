#include <whs31/common/glaze/angle.h>
#include <whs31/common/glaze/curve.h>
#include <whs31/common/glaze/matrix3x3.h>
#include <whs31/common/glaze/point2.h>
#include <whs31/common/glaze/point3.h>
#include <whs31/common/glaze/quaternion.h>
#include <whs31/common/glaze/random.h>
#include <whs31/common/glaze/vector2.h>
#include <whs31/common/glaze/vector3.h>

#include <array>

#include <catch2/catch_test_macros.hpp>

#include "glaze_support.h"

using namespace whs31;
using namespace glaze_support;

TEST_CASE("Vector and point glaze adapters use named coordinates", "[glaze][geometry]")
{
  check_round_trip(Vector2::from_xy(1.5, -2.0), R"({"x":1.5,"y":-2})");
  check_round_trip(Vector3::from_xyz(1.5, -2.0, 3.25), R"({"x":1.5,"y":-2,"z":3.25})");
  check_round_trip(Point2::from_xy(1.5, -2.0), R"({"x":1.5,"y":-2})");
  check_round_trip(Point3::from_xyz(1.5, -2.0, 3.25), R"({"x":1.5,"y":-2,"z":3.25})");
  check_round_trip(Quaternion::from_wxyz(1.0, 0.0, 0.5, -0.5), R"({"w":1,"x":0,"y":0.5,"z":-0.5})");

  REQUIRE(read_error<Vector3>(R"({"x":1,"y":2,"w":3})") == glz::error_code::unknown_key);
  REQUIRE(strict_read_error<Vector3>(R"({"x":1,"y":2})") == glz::error_code::missing_key);
}

TEST_CASE("Matrix glaze adapter writes three rows", "[glaze][matrix]")
{
  check_round_trip(Matrix3x3::identity(), "[[1,0,0],[0,1,0],[0,0,1]]");
  check_round_trip(
    Matrix3x3::from_rows({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.5}),
    "[[1,2,3],[4,5,6],[7,8,9.5]]"
  );

  REQUIRE(read_error<Matrix3x3>("[[1,0,0],[0,1,0],[0,0,1],[0,0,0]]") != glz::error_code::none);
  REQUIRE(read_error<Matrix3x3>("[[1,0,0,0],[0,1,0],[0,0,1]]") != glz::error_code::none);
  REQUIRE(read_error<Matrix3x3>(R"({"rows":[]})") != glz::error_code::none);
}

TEST_CASE("Curve glaze adapter validates the points", "[glaze][curve]")
{
  using C = Curve<double, double, 4>;
  auto const curve = C(std::array {0.0, 1.0, 2.0}, std::array {10.0, 20.0, 15.0});
  check_round_trip(curve, R"({"arguments":[0,1,2],"values":[10,20,15]})");

  auto const angles = Curve<
    Angle,
    double,
    2>(std::array {Angle::from_degrees(0.0), Angle::from_degrees(90.0)}, std::array {0.0, 1.0});
  REQUIRE(read<Curve<Angle, double, 2>>(write(angles)) == angles);

  REQUIRE(
    read_error<C>(R"({"arguments":[0,2,1],"values":[1,2,3]})")
    == glz::error_code::constraint_violated
  );
  REQUIRE(
    read_error<C>(R"({"arguments":[0,1],"values":[1]})") == glz::error_code::constraint_violated
  );
  REQUIRE(
    read_error<C>(R"({"arguments":[0,1,2,3,4],"values":[0,1,2,3,4]})")
    == glz::error_code::constraint_violated
  );
}

TEST_CASE("Random stream glaze adapter restores the generator state", "[glaze][random]")
{
  check_round_trip(RandomStream {.state = {1, 2, 3, 4}}, R"({"state":[1,2,3,4]})");

  auto stream = RandomStream::derive(1, "vt30-1", "imu.gyro");
  REQUIRE(read<RandomStream>(write(stream)) == stream);

  REQUIRE(
    read_error<RandomStream>(R"({"state":[0,0,0,0]})") == glz::error_code::constraint_violated
  );
}
