#include <whs31/common/config.h>

#ifdef WHS31_COMMON_WITH_NLOHMANN

#  include <whs31/common/geodesy.h>
#  include <whs31/common/glaze/angle.h>
#  include <whs31/common/glaze/chrono.h>
#  include <whs31/common/glaze/curve.h>
#  include <whs31/common/glaze/distance.h>
#  include <whs31/common/glaze/euler_angles.h>
#  include <whs31/common/glaze/matrix3x3.h>
#  include <whs31/common/glaze/pressure.h>
#  include <whs31/common/glaze/quaternion.h>
#  include <whs31/common/glaze/random.h>
#  include <whs31/common/glaze/sockaddr.h>
#  include <whs31/common/glaze/uuid.h>
#  include <whs31/common/glaze/vector3.h>
#  include <whs31/common/glaze/velocity.h>
#  include <whs31/common/json/angle.h>
#  include <whs31/common/json/chrono.h>
#  include <whs31/common/json/curve.h>
#  include <whs31/common/json/euler_angles.h>
#  include <whs31/common/json/geodesy.h>
#  include <whs31/common/json/matrix3x3.h>
#  include <whs31/common/json/pressure.h>
#  include <whs31/common/json/quaternion.h>
#  include <whs31/common/json/random.h>
#  include <whs31/common/json/sockaddr.h>
#  include <whs31/common/json/uuid.h>
#  include <whs31/common/json/vector3.h>
#  include <whs31/common/json/velocity.h>

#  include <array>
#  include <chrono>

#  include <nlohmann/json.hpp>

#  include <catch2/catch_template_test_macros.hpp>
#  include <catch2/catch_test_macros.hpp>

#  include "glaze_support.h"

using namespace whs31;
using namespace glaze_support;

namespace
{
  template <typename T>
  void check_interop(T const& value)
  {
    auto const from_nlohmann = nlohmann::json(value);
    REQUIRE(nlohmann::json::parse(write(value)) == from_nlohmann);
    REQUIRE(read<T>(from_nlohmann.dump()) == value);
  }
}  // namespace

TEST_CASE("Glaze and nlohmann adapters produce the same JSON", "[glaze][json]")
{
  check_interop(Angle::from_degrees(45.0));
  check_interop(Velocity::from_mps(7.25));
  check_interop(std::chrono::milliseconds(1'500));
  check_interop(std::chrono::duration<double, std::milli>(7.5));
  check_interop(Pressure::from_hectopascals(1'013.25));
  check_interop(EulerAngles::from_degrees(10.0, 20.0, 30.0));
  check_interop(Vector3::from_xyz(1.0, -2.0, 3.5));
  check_interop(Quaternion::from_wxyz(1.0, 0.0, 0.0, 0.0));
  check_interop(
    GeodeticPosition {
      .latitude = 45_deg,
      .longitude = -30.5_deg,
      .altitude_msl = 120_meters,
    }
  );
  check_interop(Matrix3x3::from_rows({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}));
  check_interop(Curve<double, double, 4>(std::array {0.0, 1.0}, std::array {2.0, 3.0}));
  check_interop(RandomStream {.state = {1, 2, 3, 4}});
  check_interop(Uuid("67e55044-10b1-426f-9247-bb680e5fe0c8"));
  check_interop(SockAddr(SockAddrV4(Ipv4Addr::parse("10.0.0.2"), 8'080)));
  check_interop(SockAddr(SockAddrV6(Ipv6Addr::parse("fe80::1"), 443, 7, 3)));
}

#endif
