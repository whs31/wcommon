#include <whs31/common/angle.h>
#include <whs31/common/distance.h>
#include <whs31/common/error.h>
#include <whs31/common/geodesy.h>
#include <whs31/common/math.h>
#include <whs31/common/vector3.h>

#include <cmath>
#include <algorithm>
#include <string>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

using namespace whs31;
using Catch::Matchers::WithinAbs;

namespace
{
  class IsCloseTo : public Catch::Matchers::MatcherGenericBase
  {
   public:
    IsCloseTo(Vector3 const& expected, double const tolerance)
      : m_expected(expected)
      , m_tolerance(tolerance)
    {}

    [[nodiscard]] bool match(Vector3 const& actual) const
    {
      return std::max({
               std::abs(actual.x() - this->m_expected.x()),
               std::abs(actual.y() - this->m_expected.y()),
               std::abs(actual.z() - this->m_expected.z()),
             })
          <= this->m_tolerance;
    }

    // Catch2 matchers override describe() publicly, as its documentation shows
    [[nodiscard]] std::string
      describe() const override  // NOLINT(misc-override-with-different-visibility)
    {
      return fmt::format("is within {} of {}", this->m_tolerance, this->m_expected);
    }

   private:
    Vector3 m_expected;
    double m_tolerance;
  };

  [[nodiscard]] IsCloseTo is_close_to(Vector3 const& expected, double const tolerance)
  {
    return {expected, tolerance};
  }

  auto constexpr origin = GeodeticPosition {
    .latitude = 60.195964_deg,
    .longitude = 30.323451_deg,
  };

  [[nodiscard]] GeodeticPosition origin_at(Distance const altitude)
  {
    auto result = origin;
    result.altitude_msl = altitude;
    return result;
  }

  [[nodiscard]] GeodeticPosition at(double const latitude_deg, double const longitude_deg)
  {
    return {
      .latitude = Angle::from_degrees(latitude_deg),
      .longitude = Angle::from_degrees(longitude_deg),
    };
  }

  // 1e-10 degrees of latitude are 11 µm
  auto constexpr angle_tolerance_deg = 1e-10;
}  // namespace

TEST_CASE("WGS84 constants derive from the axis and the flattening", "[geodesy]")
{
  CHECK(wgs84::semi_major_axis == 6'378'137_meters);
  CHECK_THAT(wgs84::semi_minor_axis.as_m(), WithinAbs(6'356'752.314'245, 1e-6));
  CHECK_THAT(wgs84::eccentricity_squared, WithinAbs(6.694'379'990'14e-3, 1e-14));
  CHECK_THAT(wgs84::second_eccentricity_squared, WithinAbs(6.739'496'742'28e-3, 1e-14));
}

TEST_CASE("Geodetic positions format in degrees and metres MSL", "[geodesy]")
{
  auto constexpr position = GeodeticPosition {
    .latitude = 59.5_deg,
    .longitude = -30.25_deg,
    .altitude_msl = 70_meters,
  };
  CHECK(position.to_string() == "59.5 deg, -30.25 deg, 70 m MSL");
  CHECK(fmt::format("{}", position) == position.to_string());
  CHECK(position.latitude_degrees() == 59.5);
  CHECK(position.longitude_degrees() == -30.25);
  CHECK(position.altitude_msl_meters() == 70.0);
  STATIC_CHECK(position == position);
  STATIC_CHECK(position != GeodeticPosition {});
  STATIC_CHECK(position.is_finite());
  CHECK_FALSE(GeodeticPosition {.altitude_msl = Distance::from_meters(NaN)}.is_finite());
  CHECK_FALSE(GeodeticPosition {.longitude = Angle::from_degrees(Inf)}.is_finite());
}

TEST_CASE("Tangent plane maps its origin onto itself", "[geodesy]")
{
  auto const plane = LocalTangentPlane(origin_at(120_meters));
  auto const position = plane.lla_from_ned({});
  CHECK_THAT(position.latitude.degrees(), WithinAbs(60.195964, angle_tolerance_deg));
  CHECK_THAT(position.longitude.degrees(), WithinAbs(30.323451, angle_tolerance_deg));
  CHECK(position.altitude_msl == 120_meters);
  CHECK_THAT(plane.ned_from_lla(plane.origin()), is_close_to(Vector3(), 1e-8));
  CHECK(LocalTangentPlane().origin() == GeodeticPosition {});
}

TEST_CASE("Tangent plane altitudes are MSL and the frame is vertically flat", "[geodesy]")
{
  auto const plane = LocalTangentPlane(origin_at(35_meters));
  CHECK(plane.altitude_msl({0.0, 0.0, -100.0}) == 135_meters);
  CHECK(plane.altitude_msl({20'000.0, -5'000.0, 10.0}) == 25_meters);

  // a vertical line keeps its latitude and longitude
  auto const low = plane.lla_from_ned({8'000.0, 6'000.0, 0.0});
  auto const high = plane.lla_from_ned({8'000.0, 6'000.0, -3'000.0});
  CHECK(low.latitude == high.latitude);
  CHECK(low.longitude == high.longitude);
  CHECK(high.altitude_msl == 3'035_meters);
  CHECK(plane.ned_from_lla(high).z() == -3'000.0);
}

TEST_CASE("Tangent plane matches the exact ellipsoid geometry", "[geodesy]")
{
  // the normal through the plane point, computed independently by iterating the latitude to
  // convergence with the same wgs84 constants
  struct Reference
  {
    Distance altitude;
    Vector3 position_ned;
    double latitude_deg;
    double longitude_deg;
  };

  auto const reference = GENERATE(
    Reference {0_meters, {10'000.0, 0.0, 0.0}, 60.285717343808926, 30.323451},
    Reference {0_meters, {0.0, 10'000.0, 0.0}, 60.19584147379489, 30.503729108885697},
    Reference {0_meters, {-7'000.0, -7'000.0, 0.0}, 60.133076018687845, 30.197496833403367},
    Reference {0_meters, {30'000.0, 40'000.0, 0.0}, 60.463236912301106, 31.050482099656485},
    Reference {250_meters, {10'000.0, -20'000.0, 0.0}, 60.28522199339605, 29.96192544745459}
  );
  CAPTURE(reference.position_ned, reference.altitude);
  auto const plane = LocalTangentPlane(origin_at(reference.altitude));

  auto const position = plane.lla_from_ned(reference.position_ned);
  CHECK_THAT(position.latitude.degrees(), WithinAbs(reference.latitude_deg, angle_tolerance_deg));
  CHECK_THAT(position.longitude.degrees(), WithinAbs(reference.longitude_deg, angle_tolerance_deg));
  // asked again, the plane answers from its memo
  CHECK(plane.lla_from_ned(reference.position_ned) == position);

  auto const back = plane.ned_from_lla({
    .latitude = Angle::from_degrees(reference.latitude_deg),
    .longitude = Angle::from_degrees(reference.longitude_deg),
    .altitude_msl = reference.altitude,
  });
  CHECK_THAT(back, is_close_to(reference.position_ned, 1e-5));
}

TEST_CASE("Tangent plane follows WGS84 geodesics within its documented error", "[geodesy]")
{
  // geodesics from the origin (Karney, geographiclib 2.1): azimuth, distance and end point. The
  // plane departs from them by s³/3R², 8 mm at 10 km and 1 m at 50 km
  struct Geodesic
  {
    Angle azimuth;
    Distance distance;
    double latitude_deg;
    double longitude_deg;
    double tolerance_m;
  };

  auto const geodesic = GENERATE(
    Geodesic {0_deg, 10'000_meters, 60.28571741722374, 30.323451, 0.01},
    Geodesic {90_deg, 10'000_meters, 60.195841473594896, 30.503729255858506, 0.01},
    Geodesic {225_deg, 10'000_meters, 60.13243687852601, 30.196220442681792, 0.01},
    Geodesic {125_deg, 10'000_meters, 60.14440092803332, 30.470895561590257, 0.01},
    Geodesic {125_deg, 50'000_meters, 59.93651725611862, 31.056067573644384, 1.1}
  );
  CAPTURE(geodesic.azimuth, geodesic.distance);
  auto const plane = LocalTangentPlane(origin);
  auto const expected = Vector3 {
    geodesic.distance.as_m() * geodesic.azimuth.cos(),
    geodesic.distance.as_m() * geodesic.azimuth.sin(),
    0.0
  };
  CHECK_THAT(
    plane.ned_from_lla(at(geodesic.latitude_deg, geodesic.longitude_deg)),
    is_close_to(expected, geodesic.tolerance_m)
  );
}

TEST_CASE("Tangent plane round trip stays within a millimetre at 50 km", "[geodesy]")
{
  auto const latitude_deg = GENERATE(0.0, 45.0, 60.195964, -70.0, 85.0);
  auto const plane = LocalTangentPlane({
    .latitude = Angle::from_degrees(latitude_deg),
    .longitude = -120.0_deg,
    .altitude_msl = 1'500_meters,
  });
  for(auto step = 0; step < 24; ++step) {
    auto const azimuth = Angle::from_degrees(15.0 * step);
    auto const position_ned = Vector3 {50'000.0 * azimuth.cos(), 50'000.0 * azimuth.sin(), -400.0};
    CAPTURE(latitude_deg, azimuth.degrees());
    CHECK_THAT(
      plane.ned_from_lla(plane.lla_from_ned(position_ned)),
      is_close_to(position_ned, 1e-3)
    );
  }
}

TEST_CASE("Tangent plane longitudes wrap at the antimeridian", "[geodesy]")
{
  auto const plane = LocalTangentPlane(at(10.0, 179.99));
  auto const east = plane.lla_from_ned({0.0, 5'000.0, 0.0});
  CHECK_THAT(east.latitude.degrees(), WithinAbs(9.999996876041306, angle_tolerance_deg));
  CHECK_THAT(east.longitude.degrees(), WithinAbs(-179.96439595088293, angle_tolerance_deg));
  CHECK_THAT(plane.ned_from_lla(east), is_close_to(Vector3 {0.0, 5'000.0, 0.0}, 1e-5));
  // the same meridian written as +180.0356°
  CHECK_THAT(
    plane.ned_from_lla(at(east.latitude.degrees(), 180.03560404911707)),
    is_close_to(Vector3 {0.0, 5'000.0, 0.0}, 1e-5)
  );
}

TEST_CASE("Tangent plane rejects invalid origins and positions", "[geodesy]")
{
  CHECK_THROWS_AS(LocalTangentPlane(at(90.0, 0.0)), InvalidArgumentError);
  CHECK_THROWS_AS(LocalTangentPlane(at(-90.0, 0.0)), InvalidArgumentError);
  CHECK_THROWS_AS(LocalTangentPlane(at(0.0, 180.5)), InvalidArgumentError);
  CHECK_THROWS_AS(LocalTangentPlane(at(NaN, 0.0)), InvalidArgumentError);
  CHECK_THROWS_AS(LocalTangentPlane(origin_at(Distance::from_meters(NaN))), InvalidArgumentError);

  auto const plane = LocalTangentPlane(origin);
  CHECK_THROWS_AS(plane.lla_from_ned({NaN, 0.0, 0.0}), InvalidArgumentError);
  CHECK_THROWS_AS(plane.lla_from_ned({0.0, 0.0, Inf}), InvalidArgumentError);
  CHECK_THROWS_AS(plane.ned_from_lla(at(NaN, 30.0)), InvalidArgumentError);
  CHECK_THROWS_AS(plane.ned_from_lla(at(91.0, 30.0)), InvalidArgumentError);
  // the antipode's normal runs parallel to the plane
  CHECK_THROWS_AS(plane.ned_from_lla(at(-60.195964, -149.676549)), NumericError);
}
