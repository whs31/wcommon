#include <whs31/common/ip.h>
#include <whs31/common/sockaddr.h>

#ifdef NLOHMANN_JSON_VERSION_MAJOR
#  error "Base networking headers must not include nlohmann_json"
#endif

#include <array>
#include <compare>
#include <cstdint>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <unordered_set>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;
using namespace std::string_view_literals;

TEST_CASE("IP address value operations are constexpr", "[network][ip]")
{
  STATIC_CHECK(Ipv4Addr {}.is_unspecified());
  STATIC_CHECK(Ipv6Addr {}.is_unspecified());
  STATIC_CHECK(Ipv4Addr::from_octets(127, 0, 0, 1).address() == 0x7f000001U);
  STATIC_CHECK(
    Ipv4Addr::from_octets(192, 168, 1, 10).octets() == std::array<std::uint8_t, 4> {192, 168, 1, 10}
  );
  STATIC_CHECK(Ipv4Addr::from_octets(127, 42, 0, 1).is_loopback());
  STATIC_CHECK(Ipv4Addr::from_octets(10, 1, 2, 3).is_private());
  STATIC_CHECK(Ipv4Addr::from_octets(172, 16, 0, 0).is_private());
  STATIC_CHECK(Ipv4Addr::from_octets(172, 31, 255, 255).is_private());
  STATIC_CHECK(Ipv4Addr::from_octets(192, 168, 1, 1).is_private());
  STATIC_CHECK_FALSE(Ipv4Addr::from_octets(172, 32, 0, 0).is_private());
  STATIC_CHECK(Ipv4Addr::from_octets(224, 0, 0, 1).is_multicast());
  STATIC_CHECK(Ipv4Addr::from_octets(239, 255, 255, 255).is_multicast());
  STATIC_CHECK_FALSE(Ipv4Addr::from_octets(240, 0, 0, 0).is_multicast());

  STATIC_CHECK(Ipv6Addr::from_segments(0, 0, 0, 0, 0, 0, 0, 1).is_loopback());
  STATIC_CHECK(Ipv6Addr::from_segments(0xff02, 0, 0, 0, 0, 0, 0, 1).is_multicast());
  STATIC_CHECK(
    Ipv6Addr::from_segments(0x2001, 0x0db8, 0, 0, 0, 0, 0, 1).segments()
    == std::array<std::uint16_t, 8> {0x2001, 0x0db8, 0, 0, 0, 0, 0, 1}
  );

  STATIC_CHECK(std::is_copy_constructible_v<Ipv4Addr>);
  STATIC_CHECK(std::is_copy_assignable_v<Ipv4Addr>);
  STATIC_CHECK(std::is_copy_constructible_v<Ipv6Addr>);
  STATIC_CHECK(std::is_copy_assignable_v<Ipv6Addr>);
}

TEST_CASE("IP address constants and literals expose common addresses", "[network][ip]")
{
  CHECK(Ipv4Addr::LOCALHOST == Ipv4Addr::from_octets(127, 0, 0, 1));
  CHECK(Ipv4Addr::UNSPECIFIED.is_unspecified());
  CHECK(Ipv4Addr::BROADCAST == Ipv4Addr::from_octets(255, 255, 255, 255));
  CHECK("127.0.0.1"_ipv4 == Ipv4Addr::LOCALHOST);
  CHECK("0.0.0.0"_ipv4 == Ipv4Addr::UNSPECIFIED);
  CHECK("255.255.255.255"_ipv4 == Ipv4Addr::BROADCAST);

  CHECK(Ipv6Addr::LOCALHOST == Ipv6Addr::from_segments(0, 0, 0, 0, 0, 0, 0, 1));
  CHECK(Ipv6Addr::UNSPECIFIED.is_unspecified());
  CHECK("::1"_ipv6 == Ipv6Addr::LOCALHOST);
  CHECK("::"_ipv6 == Ipv6Addr::UNSPECIFIED);
}

TEST_CASE("IPv4 addresses parse, format and expose octets", "[network][ip]")
{
  auto const address = Ipv4Addr::parse("192.168.1.10");

  CHECK(address == Ipv4Addr::from_octets(192, 168, 1, 10));
  CHECK(address.octets() == std::array<std::uint8_t, 4> {192, 168, 1, 10});
  CHECK(address.to_string() == "192.168.1.10");
  CHECK(address.is_private());
  CHECK_FALSE(address.is_loopback());
  CHECK_FALSE(address.is_unspecified());

  auto stream = std::ostringstream();
  stream << address;
  CHECK(stream.str() == "192.168.1.10");
  CHECK(fmt::format("{}", address) == "192.168.1.10");
}

TEST_CASE("IPv4 parser rejects malformed and non-IPv4 input", "[network][ip]")
{
  for(auto const text : std::array {
        std::string_view {},
        std::string_view {"127.0.0"},
        std::string_view {"127.0.0.1."},
        std::string_view {"256.0.0.1"},
        std::string_view {"+127.0.0.1"},
        std::string_view {" 127.0.0.1"},
        std::string_view {"::1"},
      }) {
    CAPTURE(text);
    CHECK_THROWS_AS(Ipv4Addr::parse(text), IpError);
  }

  CHECK_THROWS_AS(Ipv4Addr::parse("127.0.0.1\0junk"sv), IpError);
}

TEST_CASE("IPv6 addresses parse, format and expose segments", "[network][ip]")
{
  auto const address = Ipv6Addr::parse("2001:0db8:0:0:0:0:0:1");

  CHECK(address.segments() == std::array<std::uint16_t, 8> {0x2001, 0x0db8, 0, 0, 0, 0, 0, 1});
  CHECK(address.to_string() == "2001:db8::1");
  CHECK_FALSE(address.is_loopback());
  CHECK_FALSE(address.is_unspecified());
  CHECK_FALSE(address.is_multicast());
  CHECK(Ipv6Addr::parse("::1").is_loopback());
  CHECK(Ipv6Addr::parse("ff02::1").is_multicast());
  CHECK(fmt::format("{}", address) == "2001:db8::1");

  auto stream = std::ostringstream();
  stream << address;
  CHECK(stream.str() == "2001:db8::1");
}

TEST_CASE("IPv6 parser rejects malformed and non-IPv6 input", "[network][ip]")
{
  for(auto const text : std::array {
        std::string_view {},
        std::string_view {":"},
        std::string_view {"2001:db8:::1"},
        std::string_view {"gggg::1"},
        std::string_view {"2001:db8::1 "},
        std::string_view {"192.168.1.10"},
      }) {
    CAPTURE(text);
    CHECK_THROWS_AS(Ipv6Addr::parse(text), IpError);
  }

  CHECK_THROWS_AS(Ipv6Addr::parse("2001:db8::1\0junk"sv), IpError);
}

TEST_CASE("IP addresses support ordering and hashing", "[network][ip]")
{
  auto const ipv4_first = Ipv4Addr::parse("10.0.0.1");
  auto const ipv4_second = Ipv4Addr::parse("10.0.0.2");
  auto const ipv6_first = Ipv6Addr::parse("2001:db8::1");
  auto const ipv6_second = Ipv6Addr::parse("2001:db8::2");

  CHECK((ipv4_first <=> ipv4_second) == std::strong_ordering::less);
  CHECK((ipv6_first <=> ipv6_second) == std::strong_ordering::less);
  CHECK(std::unordered_set<Ipv4Addr> {ipv4_first, ipv4_second, ipv4_first}.size() == 2);
  CHECK(std::unordered_set<Ipv6Addr> {ipv6_first, ipv6_second, ipv6_first}.size() == 2);
}

TEST_CASE("Socket address value operations are constexpr", "[network][sockaddr]")
{
  auto constexpr v4 = SockAddrV4(Ipv4Addr::from_octets(127, 0, 0, 1), 8'080);
  auto constexpr v6 = SockAddrV6(Ipv6Addr::from_segments(0, 0, 0, 0, 0, 0, 0, 1), 443, 12, 34);

  STATIC_CHECK(v4.port() == 8'080);
  STATIC_CHECK(v6.port() == 443);
  STATIC_CHECK(v6.flow_info() == 12);
  STATIC_CHECK(v6.scope_id() == 34);
  STATIC_CHECK(SockAddrV4::from_parts(Ipv4Addr::from_octets(127, 0, 0, 1), 80).port() == 80);
  STATIC_CHECK(
    SockAddrV6::from_parts(Ipv6Addr::from_segments(0, 0, 0, 0, 0, 0, 0, 1), 443).ip().is_loopback()
  );
  STATIC_CHECK(std::is_copy_constructible_v<SockAddrV4>);
  STATIC_CHECK(std::is_copy_assignable_v<SockAddrV4>);
  STATIC_CHECK(std::is_copy_constructible_v<SockAddrV6>);
  STATIC_CHECK(std::is_copy_assignable_v<SockAddrV6>);
  STATIC_CHECK(std::is_copy_constructible_v<SockAddr>);
  STATIC_CHECK(std::is_copy_assignable_v<SockAddr>);
}

TEST_CASE("Socket addresses parse and preserve their family", "[network][sockaddr]")
{
  auto const v4 = SockAddrV4::parse("127.0.0.1:8080");
  auto const v6 = SockAddrV6::parse("[::1]:443");
  auto const any_v4 = SockAddr::parse("127.0.0.1:8080");
  auto const any_v6 = SockAddr::parse("[::1]:443");

  CHECK(v4.ip().is_loopback());
  CHECK(v4.port() == 8'080);
  CHECK(v4.to_string() == "127.0.0.1:8080");
  CHECK(v6.ip().is_loopback());
  CHECK(v6.port() == 443);
  CHECK(v6.to_string() == "[::1]:443");
  CHECK(any_v4.family() == SockAddr::Family::Ipv4);
  CHECK(any_v4.as_v4() == v4);
  CHECK(any_v6.family() == SockAddr::Family::Ipv6);
  CHECK(any_v6.as_v6() == v6);
  CHECK(any_v6.port() == 443);
  CHECK(SockAddr {}.family() == SockAddr::Family::Ipv4);
  CHECK(SockAddr {}.port() == 0);
  CHECK_THROWS_AS(any_v4.as_v6(), IpError);
  CHECK_THROWS_AS(any_v6.as_v4(), IpError);
}

TEST_CASE("Socket address parser validates ports and syntax", "[network][sockaddr]")
{
  CHECK(SockAddrV4::parse("0.0.0.0:0").port() == 0);
  CHECK(SockAddrV4::parse("255.255.255.255:65535").port() == 65'535);
  CHECK(SockAddrV6::parse("[::]:0").port() == 0);
  CHECK(SockAddrV6::parse("[::1]:65535").port() == 65'535);

  for(auto const text : std::array {
        std::string_view {"127.0.0.1"},
        std::string_view {"127.0.0.1:"},
        std::string_view {"127.0.0.1:65536"},
        std::string_view {"127.0.0.1:-1"},
        std::string_view {"127.0.0.1:+1"},
        std::string_view {"127.0.0.1: 1"},
        std::string_view {"127.0.0.1:1x"},
      }) {
    CAPTURE(text);
    CHECK_THROWS_AS(SockAddrV4::parse(text), IpError);
  }

  for(auto const text : std::array {
        std::string_view {"::1:443"},
        std::string_view {"[::1]"},
        std::string_view {"[::1]:"},
        std::string_view {"[::1]:65536"},
        std::string_view {"[::1]::443"},
        std::string_view {"[::1]:443x"},
      }) {
    CAPTURE(text);
    CHECK_THROWS_AS(SockAddrV6::parse(text), IpError);
  }
}

TEST_CASE("Socket addresses support formatting ordering and hashing", "[network][sockaddr]")
{
  auto const v4 = SockAddrV4::parse("10.0.0.1:1234");
  auto const v6 = SockAddrV6(Ipv6Addr::parse("2001:db8::1"), 443, 12, 34);
  auto const any_v4 = SockAddr(v4);
  auto const any_v6 = SockAddr(v6);

  auto stream = std::ostringstream();
  stream << any_v6;

  CHECK(stream.str() == "[2001:db8::1]:443");
  CHECK(fmt::format("{}", v4) == "10.0.0.1:1234");
  CHECK(fmt::format("{}", v6) == "[2001:db8::1]:443");
  CHECK(SockAddrV4::parse("10.0.0.1:1234") == v4);
  CHECK((v4 <=> SockAddrV4::parse("10.0.0.1:1235")) == std::strong_ordering::less);
  CHECK(
    (v6 <=> SockAddrV6(Ipv6Addr::parse("2001:db8::1"), 443, 12, 35)) == std::strong_ordering::less
  );
  CHECK((any_v4 <=> any_v6) == std::strong_ordering::less);
  CHECK(std::unordered_set<SockAddrV4> {v4, v4}.size() == 1);
  CHECK(std::unordered_set<SockAddrV6> {v6, v6}.size() == 1);
  CHECK(std::unordered_set<SockAddr> {any_v4, any_v6, any_v4}.size() == 2);
}
