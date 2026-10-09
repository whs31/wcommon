#include <radar/mms/common/config.h>

#ifdef RCOMMON_WITH_NLOHMANN

#  include <radar/mms/common/glaze/angle.h>
#  include <radar/mms/common/glaze/curve.h>
#  include <radar/mms/common/glaze/euler_angles.h>
#  include <radar/mms/common/glaze/matrix3x3.h>
#  include <radar/mms/common/glaze/pressure.h>
#  include <radar/mms/common/glaze/quaternion.h>
#  include <radar/mms/common/glaze/random.h>
#  include <radar/mms/common/glaze/sockaddr.h>
#  include <radar/mms/common/glaze/uuid.h>
#  include <radar/mms/common/glaze/vector3.h>
#  include <radar/mms/common/glaze/velocity.h>
#  include <radar/mms/common/json/angle.h>
#  include <radar/mms/common/json/curve.h>
#  include <radar/mms/common/json/euler_angles.h>
#  include <radar/mms/common/json/matrix3x3.h>
#  include <radar/mms/common/json/pressure.h>
#  include <radar/mms/common/json/quaternion.h>
#  include <radar/mms/common/json/random.h>
#  include <radar/mms/common/json/sockaddr.h>
#  include <radar/mms/common/json/uuid.h>
#  include <radar/mms/common/json/vector3.h>
#  include <radar/mms/common/json/velocity.h>

#  include <array>

#  include <nlohmann/json.hpp>

#  include <catch2/catch_template_test_macros.hpp>
#  include <catch2/catch_test_macros.hpp>

#  include "glaze_support.h"

using namespace radar::mms;
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
  check_interop(Pressure::from_hectopascals(1'013.25));
  check_interop(EulerAngles::from_degrees(10.0, 20.0, 30.0));
  check_interop(Vector3::from_xyz(1.0, -2.0, 3.5));
  check_interop(Quaternion::from_wxyz(1.0, 0.0, 0.0, 0.0));
  check_interop(Matrix3x3::from_rows({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}));
  check_interop(Curve<double, double, 4>(std::array {0.0, 1.0}, std::array {2.0, 3.0}));
  check_interop(RandomStream {.state = {1, 2, 3, 4}});
  check_interop(Uuid("67e55044-10b1-426f-9247-bb680e5fe0c8"));
  check_interop(SockAddr(SockAddrV4(Ipv4Addr::parse("10.0.0.2"), 8'080)));
  check_interop(SockAddr(SockAddrV6(Ipv6Addr::parse("fe80::1"), 443, 7, 3)));
}

#endif
