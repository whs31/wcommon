#include <whs31/common/glaze/ip.h>
#include <whs31/common/glaze/sockaddr.h>
#include <whs31/common/glaze/uuid.h>

#include <catch2/catch_test_macros.hpp>

#include "glaze_support.h"

using namespace whs31;
using namespace glaze_support;

TEST_CASE("UUID glaze adapter uses the canonical string", "[glaze][uuid]")
{
  auto const uuid = Uuid("67e55044-10b1-426f-9247-bb680e5fe0c8");
  check_round_trip(uuid, R"("67e55044-10b1-426f-9247-bb680e5fe0c8")");
  check_round_trip(Uuid(), R"("00000000-0000-0000-0000-000000000000")");

  REQUIRE(read_error<Uuid>(R"("not-a-uuid")") == glz::error_code::constraint_violated);
  REQUIRE(read_error<Uuid>("42") != glz::error_code::none);
}

TEST_CASE("IP address glaze adapters use their textual form", "[glaze][network]")
{
  check_round_trip(Ipv4Addr::parse("192.168.0.1"), R"("192.168.0.1")");
  check_round_trip(Ipv6Addr::parse("::1"), R"("::1")");

  REQUIRE(read_error<Ipv4Addr>(R"("300.0.0.1")") == glz::error_code::constraint_violated);
  REQUIRE(read_error<Ipv6Addr>(R"("::g")") == glz::error_code::constraint_violated);
}

TEST_CASE("Socket address glaze adapters match the JSON schema", "[glaze][network]")
{
  auto const v4 = SockAddrV4(Ipv4Addr::parse("10.0.0.2"), 8'080);
  auto const v6 = SockAddrV6(Ipv6Addr::parse("fe80::1"), 443, 7, 3);

  check_round_trip(v4, R"({"ip":"10.0.0.2","port":8080})");
  check_round_trip(v6, R"({"ip":"fe80::1","port":443,"flow_info":7,"scope_id":3})");
  check_round_trip(SockAddr(v4), R"({"family":"ipv4","address":{"ip":"10.0.0.2","port":8080}})");
  check_round_trip(
    SockAddr(v6),
    R"({"family":"ipv6","address":{"ip":"fe80::1","port":443,"flow_info":7,"scope_id":3}})"
  );

  REQUIRE(
    read<SockAddr>(R"({"family":"ipv6","address":{"ip":"::1","port":1}})")
    == SockAddr(SockAddrV6(Ipv6Addr::parse("::1"), 1, 0, 0))
  );
  REQUIRE(read_error<SockAddrV4>(R"({"ip":"10.0.0.2","port":70000})") != glz::error_code::none);
  REQUIRE(read_error<SockAddrV4>(R"({"ip":"10.0.0.2","port":-1})") != glz::error_code::none);
  REQUIRE(
    read_error<SockAddr>(R"({"family":"ipx","address":{"ip":"10.0.0.2","port":1}})")
    == glz::error_code::constraint_violated
  );
  REQUIRE(
    read_error<SockAddr>(R"({"family":"ipv4","address":{"ip":"::1","port":1}})")
    == glz::error_code::constraint_violated
  );
  REQUIRE(
    read_error<SockAddr>(R"({"family":"ipv4","address":{"ip":"10.0.0.2","port":1,"scope_id":1}})")
    == glz::error_code::constraint_violated
  );
}
