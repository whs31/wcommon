#include <radar/mms/common/ip.h>
#include <radar/mms/common/sockaddr.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt configuration must enable RCOMMON_WITH_QT"
#endif

#include <qbytearrayview.h>
#include <qhostaddress.h>
#include <qmetaobject.h>
#include <qmetatype.h>
#include <qstring.h>
#include <qvariant.h>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("IP addresses expose Qt conversions and metatypes", "[network][qt][ip]")
{
  STATIC_CHECK(QMetaTypeId2<Ipv4Addr>::Defined);
  STATIC_CHECK(QMetaTypeId2<Ipv6Addr>::Defined);

  auto const ipv4 = Ipv4Addr::from_qstring(QStringView {u"192.168.1.10"});
  auto const ipv6 = Ipv6Addr::from_qstring(QString {"2001:db8::1"});

  CHECK(ipv4.to_qstring() == QString {"192.168.1.10"});
  CHECK(ipv6.to_qstring() == QString {"2001:db8::1"});
  CHECK(Ipv4Addr::from_qhostaddress(ipv4.to_qhostaddress()) == ipv4);
  CHECK(Ipv6Addr::from_qhostaddress(ipv6.to_qhostaddress()) == ipv6);
  CHECK_THROWS_AS(Ipv4Addr::from_qhostaddress(QHostAddress {"::1"}), IpError);
  CHECK_THROWS_AS(Ipv6Addr::from_qhostaddress(QHostAddress {"127.0.0.1"}), IpError);

  auto const ipv4_type = QMetaType::fromType<Ipv4Addr>();
  auto const ipv6_type = QMetaType::fromType<Ipv6Addr>();
  CHECK(ipv4_type.isValid());
  CHECK(ipv6_type.isValid());
  CHECK(QByteArrayView {ipv4_type.name()}.contains("Ipv4Addr"));
  CHECK(QByteArrayView {ipv6_type.name()}.contains("Ipv6Addr"));
  CHECK(QVariant::fromValue(ipv4).value<Ipv4Addr>() == ipv4);
  CHECK(QVariant::fromValue(ipv6).value<Ipv6Addr>() == ipv6);
}

TEST_CASE("IP address gadgets expose their declared properties", "[network][qt][ip]")
{
  auto const ipv4 = Ipv4Addr::parse("192.168.1.10");
  auto const ipv6 = Ipv6Addr::parse("::1");
  auto const ipv4_string = Ipv4Addr::staticMetaObject.property(
    Ipv4Addr::staticMetaObject.indexOfProperty("string")
  );
  auto const ipv4_private = Ipv4Addr::staticMetaObject.property(
    Ipv4Addr::staticMetaObject.indexOfProperty("isPrivate")
  );
  auto const ipv6_loopback = Ipv6Addr::staticMetaObject.property(
    Ipv6Addr::staticMetaObject.indexOfProperty("isLoopback")
  );

  REQUIRE(ipv4_string.isValid());
  REQUIRE(ipv4_private.isValid());
  REQUIRE(ipv6_loopback.isValid());
  CHECK(ipv4_string.readOnGadget(&ipv4).toString() == QString {"192.168.1.10"});
  CHECK(ipv4_private.readOnGadget(&ipv4).toBool());
  CHECK(ipv6_loopback.readOnGadget(&ipv6).toBool());
}

TEST_CASE(
  "Socket addresses expose Qt conversions metatypes and properties",
  "[network][qt][sockaddr]"
)
{
  STATIC_CHECK(QMetaTypeId2<SockAddrV4>::Defined);
  STATIC_CHECK(QMetaTypeId2<SockAddrV6>::Defined);
  STATIC_CHECK(QMetaTypeId2<SockAddr>::Defined);

  auto const v4 = SockAddrV4::from_qstring(QStringView {u"127.0.0.1:8080"});
  auto const v6 = SockAddrV6::from_qstring(QString {"[::1]:443"});
  auto const any = SockAddr::from_qstring(QString {"[::1]:443"});
  auto const port_property = SockAddrV4::staticMetaObject.property(
    SockAddrV4::staticMetaObject.indexOfProperty("port")
  );
  auto const family_property = SockAddr::staticMetaObject.property(
    SockAddr::staticMetaObject.indexOfProperty("family")
  );

  CHECK(v4.to_qstring() == QString {"127.0.0.1:8080"});
  CHECK(v6.to_qstring() == QString {"[::1]:443"});
  CHECK(any.to_qstring() == QString {"[::1]:443"});
  CHECK(QVariant::fromValue(v4).value<SockAddrV4>() == v4);
  CHECK(QVariant::fromValue(v6).value<SockAddrV6>() == v6);
  CHECK(QVariant::fromValue(any).value<SockAddr>() == any);
  REQUIRE(port_property.isValid());
  REQUIRE(family_property.isValid());
  CHECK(port_property.readOnGadget(&v4).toUInt() == 8'080);
  CHECK(family_property.readOnGadget(&any).value<SockAddr::Family>() == SockAddr::Family::Ipv6);
}
