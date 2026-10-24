#include <whs31/common/angle.h>
#include <whs31/common/distance.h>
#include <whs31/common/web_mercator.h>

#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace whs31;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

TEST_CASE("Web Mercator maps the world onto a square of tiles", "[web_mercator]")
{
  STATIC_CHECK(web_mercator::tiles(0) == 1.0);
  STATIC_CHECK(web_mercator::tiles(12) == 4'096.0);
  STATIC_CHECK(web_mercator::tiles(30) == 1'073'741'824.0);

  CHECK(web_mercator::project(0_deg, 0_deg, 0) == web_mercator::Point {.x = 0.5, .y = 0.5});
  CHECK(web_mercator::project(0_deg, -180_deg, 1) == web_mercator::Point {.x = 0.0, .y = 1.0});
  CHECK_THAT(web_mercator::project(web_mercator::max_latitude, 0_deg, 4).y, WithinAbs(0.0, 1e-9));
  CHECK_THAT(web_mercator::project(-web_mercator::max_latitude, 0_deg, 4).y, WithinAbs(16.0, 1e-9));
  // the edges of the map are the latitude whose row is zero
  CHECK_THAT(
    web_mercator::latitude_at(0.0, 7).degrees(),
    WithinAbs(web_mercator::max_latitude.degrees(), 1e-9)
  );
}

TEST_CASE("Web Mercator places a point in its slippy map tile", "[web_mercator]")
{
  auto const at = web_mercator::project(60.2_deg, 30.3_deg, 12);
  CHECK_THAT(at.x, WithinAbs(2'392.746'666'666'667, 1e-9));
  CHECK_THAT(at.y, WithinAbs(1'184.911'994'830'207, 1e-9));
}

TEST_CASE("Web Mercator clamps latitudes beyond the map", "[web_mercator]")
{
  CHECK(
    web_mercator::project(89_deg, 10_deg, 3)
    == web_mercator::project(web_mercator::max_latitude, 10_deg, 3)
  );
  CHECK(
    web_mercator::project(-90_deg, 10_deg, 3)
    == web_mercator::project(-web_mercator::max_latitude, 10_deg, 3)
  );
  CHECK(
    web_mercator::tile_size(90_deg, 3) == web_mercator::tile_size(web_mercator::max_latitude, 3)
  );
}

TEST_CASE("Web Mercator inverts its projection", "[web_mercator]")
{
  auto const latitude_deg = GENERATE(-85.0, -45.5, 0.0, 30.25, 60.195964, 85.0);
  auto const longitude_deg = GENERATE(-180.0, -73.5, 0.0, 30.323451, 179.75);
  auto const zoom = GENERATE(std::uint8_t {0}, std::uint8_t {9}, std::uint8_t {17});
  CAPTURE(latitude_deg, longitude_deg, zoom);

  auto const at = web_mercator::
    project(Angle::from_degrees(latitude_deg), Angle::from_degrees(longitude_deg), zoom);
  CHECK_THAT(web_mercator::latitude_at(at.y, zoom).degrees(), WithinAbs(latitude_deg, 1e-9));
  CHECK_THAT(web_mercator::longitude_at(at.x, zoom).degrees(), WithinAbs(longitude_deg, 1e-9));
}

TEST_CASE("Web Mercator tiles shrink towards the poles", "[web_mercator]")
{
  CHECK_THAT(web_mercator::tile_size(0_deg, 0).as_m(), WithinRel(40'075'016.685'578'49, 1e-12));
  CHECK_THAT(web_mercator::tile_size(60.2_deg, 12).as_m(), WithinRel(4'862.363'227'655'594, 1e-12));
  CHECK(web_mercator::tile_size(-60.2_deg, 12) == web_mercator::tile_size(60.2_deg, 12));
  CHECK_THAT(
    web_mercator::tile_size(60_deg, 5).as_m(),
    WithinRel(web_mercator::tile_size(0_deg, 5).as_m() / 2.0, 1e-12)
  );
}
