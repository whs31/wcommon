#include <radar/mms/common/error.h>
#include <radar/mms/common/json/angle.h>
#include <radar/mms/common/json/curve.h>
#include <radar/mms/common/json/matrix3x3.h>
#include <radar/mms/common/json/quaternion.h>
#include <radar/mms/common/json/random.h>

#include <array>
#include <tuple>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms::common;

TEST_CASE("Matrix JSON adapter round-trips rows", "[linalg][json]")
{
  auto constexpr matrix = Matrix3x3::from_rows({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.5});

  REQUIRE(nlohmann::json(matrix) == nlohmann::json::parse("[[1, 2, 3], [4, 5, 6], [7, 8, 9.5]]"));
  REQUIRE(nlohmann::json(matrix).get<Matrix3x3>() == matrix);
  REQUIRE_THROWS_AS(
    nlohmann::json::parse("[[1, 2, 3], [4, 5, 6]]").get<Matrix3x3>(),
    InvalidArgumentError
  );
  REQUIRE_THROWS_AS(
    nlohmann::json::parse("[[1, 2, 3], [4, 5, 6], [7, 8]]").get<Matrix3x3>(),
    InvalidArgumentError
  );
  REQUIRE_THROWS_AS(nlohmann::json::parse(R"({"rows": 3})").get<Matrix3x3>(), InvalidArgumentError);
  REQUIRE_THROWS(nlohmann::json::parse(R"([[1, 2, 3], [4, 5, 6], [7, 8, "9"]])").get<Matrix3x3>());
}

TEST_CASE("Quaternion JSON adapter round-trips components", "[linalg][json]")
{
  auto constexpr quaternion = Quaternion(0.5, -0.5, 0.25, 1.0);

  REQUIRE(
    nlohmann::json(quaternion) == nlohmann::json({{"w", 0.5}, {"x", -0.5}, {"y", 0.25}, {"z", 1.0}})
  );
  REQUIRE(nlohmann::json(quaternion).get<Quaternion>() == quaternion);
  REQUIRE_THROWS(nlohmann::json({{"x", 0.0}, {"y", 0.0}, {"z", 0.0}}).get<Quaternion>());
}

TEST_CASE("Curve JSON adapter round-trips and validates points", "[curve][json]")
{
  using C = Curve<double, double, 4>;

  auto const curve = C(std::array {0.0, 0.5, 1.0}, std::array {10.0, 20.0, 30.0});
  auto const json = nlohmann::json(curve);

  REQUIRE(json == nlohmann::json::parse(R"({"arguments": [0, 0.5, 1], "values": [10, 20, 30]})"));
  REQUIRE(json.get<C>() == curve);
  REQUIRE_THROWS_AS(
    nlohmann::json::parse(R"({"arguments": [1, 0], "values": [10, 20]})").get<C>(),
    InvalidArgumentError
  );

  auto const angles = Curve<Angle, double, 2>(std::array {0_deg, 90_deg}, std::array {0.0, 1.0});
  REQUIRE(nlohmann::json(angles).get<Curve<Angle, double, 2>>() == angles);
}

TEST_CASE("Random stream JSON adapter round-trips its state", "[random][json]")
{
  auto stream = RandomStream::derive(1, "vt30-1", "imu.gyro");
  std::ignore = stream.next();

  REQUIRE(
    nlohmann::json(RandomStream {.state = {1, 2, 3, 4}})
    == nlohmann::json::parse(R"({"state": [1, 2, 3, 4]})")
  );
  REQUIRE(nlohmann::json(stream).get<RandomStream>() == stream);
  REQUIRE_THROWS_AS(
    nlohmann::json::parse(R"({"state": [0, 0, 0, 0]})").get<RandomStream>(),
    InvalidArgumentError
  );
}
