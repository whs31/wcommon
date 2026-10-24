#include <whs31/common/json/ip.h>
#include <whs31/common/json/sockaddr.h>

#include <cstdint>
#include <limits>
#include <string>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

TEST_CASE("IP addresses serialize as JSON strings", "[network][json][ip]")
{
  auto const ipv4 = Ipv4Addr::parse("10.0.0.1");
  auto const ipv6 = Ipv6Addr::parse("2001:db8::1");

  CHECK(nlohmann::json(ipv4) == nlohmann::json("10.0.0.1"));
  CHECK(nlohmann::json(ipv6) == nlohmann::json("2001:db8::1"));
  CHECK(nlohmann::json("10.0.0.1").get<Ipv4Addr>() == ipv4);
  CHECK(nlohmann::json("2001:db8::1").get<Ipv6Addr>() == ipv6);
  CHECK_THROWS(nlohmann::json(42).get<Ipv4Addr>());
}

TEST_CASE("Concrete socket addresses use lossless JSON objects", "[network][json][sockaddr]")
{
  auto const v4 = SockAddrV4(Ipv4Addr::parse("10.0.0.1"), 1'234);
  auto const v6 = SockAddrV6(Ipv6Addr::parse("2001:db8::1"), 443, 12, 34);

  auto const expected_v4 = nlohmann::json {
    {"ip", "10.0.0.1"},
    {"port", 1'234},
  };
  auto const expected_v6 = nlohmann::json {
    {"ip", "2001:db8::1"},
    {"port", 443},
    {"flow_info", 12},
    {"scope_id", 34},
  };

  CHECK(nlohmann::json(v4) == expected_v4);
  CHECK(nlohmann::json(v6) == expected_v6);
  CHECK(expected_v4.get<SockAddrV4>() == v4);
  CHECK(expected_v6.get<SockAddrV6>() == v6);
}

TEST_CASE("Type-erased socket addresses carry an explicit JSON family", "[network][json][sockaddr]")
{
  auto const v4 = SockAddr(SockAddrV4(Ipv4Addr::parse("127.0.0.1"), 8'080));
  auto const v6 = SockAddr(SockAddrV6(Ipv6Addr::parse("::1"), 443, 7, 9));

  auto const expected_v4 = nlohmann::json {
    {"family", "ipv4"},
    {"address", nlohmann::json {{"ip", "127.0.0.1"}, {"port", 8'080}}},
  };
  auto const expected_v6 = nlohmann::json {
    {"family", "ipv6"},
    {"address",
     nlohmann::json {
       {"ip", "::1"},
       {"port", 443},
       {"flow_info", 7},
       {"scope_id", 9},
     }},
  };

  CHECK(nlohmann::json(v4) == expected_v4);
  CHECK(nlohmann::json(v6) == expected_v6);
  CHECK(expected_v4.get<SockAddr>() == v4);
  CHECK(expected_v6.get<SockAddr>() == v6);
}

TEST_CASE("Socket JSON rejects the legacy string schema", "[network][json][sockaddr]")
{
  CHECK_THROWS(nlohmann::json("127.0.0.1:8080").get<SockAddrV4>());
  CHECK_THROWS(nlohmann::json("[::1]:443").get<SockAddrV6>());
  CHECK_THROWS(nlohmann::json("127.0.0.1:8080").get<SockAddr>());
}

TEST_CASE("Socket JSON validates its schema and numeric ranges", "[network][json][sockaddr]")
{
  CHECK_THROWS(nlohmann::json({{"ip", "127.0.0.1"}}).get<SockAddrV4>());
  CHECK_THROWS(nlohmann::json({{"ip", "127.0.0.1"}, {"port", -1}}).get<SockAddrV4>());
  CHECK_THROWS(nlohmann::json({{"ip", "127.0.0.1"}, {"port", 65'536}}).get<SockAddrV4>());
  CHECK_THROWS(nlohmann::json({{"ip", "127.0.0.1"}, {"port", "8080"}}).get<SockAddrV4>());
  CHECK_THROWS(nlohmann::json({{"ip", "127.0.0.1"}, {"port", 1.5}}).get<SockAddrV4>());
  CHECK_THROWS(nlohmann::json({{"ip", "127.0.0.1"}, {"port", true}}).get<SockAddrV4>());
  CHECK_THROWS(nlohmann::json({{"ip", "::1"}, {"port", 443}, {"scope_id", 0}}).get<SockAddrV6>());
  CHECK_THROWS(nlohmann::json({{"ip", "::1"}, {"port", 443}, {"flow_info", 0}}).get<SockAddrV6>());
  CHECK_THROWS(
    nlohmann::json({{"ip", "::1"}, {"port", 443}, {"flow_info", -1}, {"scope_id", 0}})
      .get<SockAddrV6>()
  );
  CHECK_THROWS(
    nlohmann::json({{"ip", "::1"},
                    {"port", 443},
                    {"flow_info", std::numeric_limits<std::uint64_t>::max()},
                    {"scope_id", 0}})
      .get<SockAddrV6>()
  );
  CHECK_THROWS(
    nlohmann::json({{"family", "ipv5"}, {"address", nlohmann::json::object()}}).get<SockAddr>()
  );
  CHECK_THROWS(
    nlohmann::json({{"family", 4}, {"address", nlohmann::json::object()}}).get<SockAddr>()
  );
  CHECK_THROWS(nlohmann::json({{"family", "ipv4"}}).get<SockAddr>());
}
