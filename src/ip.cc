#include <whs31/common/ip.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <iterator>
#include <string>

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <arpa/inet.h>
#endif

namespace
{
  [[nodiscard]] int parse_address(int const family, char const* const text, void* const output)
  {
#ifdef _WIN32
    return InetPtonA(family, text, output);
#else
    return inet_pton(family, text, output);
#endif
  }

  [[nodiscard]] char const* format_address(
    int const family,
    void const* const address,
    char* const output,
    std::size_t const size
  )
  {
#ifdef _WIN32
    return InetNtopA(family, const_cast<void*>(address), output, size);  // NOLINT(*-const-cast)
#else
    return inet_ntop(family, address, output, static_cast<socklen_t>(size));
#endif
  }
}  // namespace

WHS31_COMMON_NAMESPACE_BEGIN

Ipv4Addr const Ipv4Addr::LOCALHOST = Ipv4Addr::from_octets(127, 0, 0, 1);
Ipv4Addr const Ipv4Addr::UNSPECIFIED = Ipv4Addr::from_octets(0, 0, 0, 0);
Ipv4Addr const Ipv4Addr::BROADCAST = Ipv4Addr::from_octets(255, 255, 255, 255);

Ipv6Addr const Ipv6Addr::LOCALHOST = Ipv6Addr::from_segments(0, 0, 0, 0, 0, 0, 0, 1);
Ipv6Addr const Ipv6Addr::UNSPECIFIED = Ipv6Addr::from_segments(0, 0, 0, 0, 0, 0, 0, 0);

IpError IpError::invalid_v4_string(std::string_view const text)
{
  return IpError("invalid ipv4 string: {}", text);
}

IpError IpError::invalid_v6_string(std::string_view const text)
{
  return IpError("invalid ipv6 string: {}", text);
}

#ifdef WHS31_COMMON_WITH_QT
IpError IpError::invalid_qhostaddress_v4()
{
  return IpError("QHostAddress does not contain an ipv4");
}

IpError IpError::invalid_qhostaddress_v6()
{
  return IpError("QHostAddress does not contain an ipv6");
}
#endif

IpError IpError::missing_colon(std::string_view const text)
{
  return IpError("missing colon in a socket address: {}", text);
}

IpError IpError::invalid_port(std::string_view const text)
{
  return IpError("invalid port: {}", text);
}

IpError IpError::not_a_v4() { return IpError("an attempt to cast an ipv6 address to ipv4"); }

IpError IpError::not_a_v6() { return IpError("an attempt to cast an ipv4 address to ipv6"); }

Ipv4Addr Ipv4Addr::parse(std::string_view const text)
{
  if(text.empty() or text.contains('\0'))
    throw IpError::invalid_v4_string(text);

  auto native = in_addr {};
  auto const terminated = std::string(text);
  if(parse_address(AF_INET, terminated.c_str(), &native) != 1)
    throw IpError::invalid_v4_string(text);

  auto octets = std::array<std::uint8_t, 4> {};
  std::memcpy(octets.data(), &native, octets.size());
  return from_octets(octets.at(0), octets.at(1), octets.at(2), octets.at(3));
}

std::string Ipv4Addr::to_string() const
{
  auto const [a, b, c, d] = this->octets();
  return fmt::format("{}.{}.{}.{}", a, b, c, d);
}

Ipv6Addr Ipv6Addr::parse(std::string_view const text)
{
  if(text.empty() or text.contains('\0'))
    throw IpError::invalid_v6_string(text);

  auto native = in6_addr {};
  auto const terminated = std::string(text);
  if(parse_address(AF_INET6, terminated.c_str(), &native) != 1)
    throw IpError::invalid_v6_string(text);

  auto octets = std::array<std::uint8_t, 16> {};
  std::memcpy(octets.data(), &native, octets.size());
  return Ipv6Addr(octets);
}

std::string Ipv6Addr::to_string() const
{
  auto buffer = std::array<char, INET6_ADDRSTRLEN> {};
  if(format_address(AF_INET6, this->m_octets.data(), buffer.data(), buffer.size()) == nullptr)
    throw IpError("failed to format ipv6 address");
  return buffer.data();
}

std::ostream& operator<<(std::ostream& stream, Ipv4Addr const& address)
{
  return stream << address.to_string();
}

std::ostream& operator<<(std::ostream& stream, Ipv6Addr const& address)
{
  return stream << address.to_string();
}

#ifdef WHS31_COMMON_WITH_QT
Ipv4Addr Ipv4Addr::from_qstring(QStringView const text)
{
  return parse(text.toString().toStdString());
}

Ipv4Addr Ipv4Addr::from_qstring(QString const& text) { return from_qstring(QStringView {text}); }

Ipv4Addr Ipv4Addr::from_qhostaddress(QHostAddress const& host)
{
  if(host.protocol() != QAbstractSocket::IPv4Protocol)
    throw IpError::invalid_qhostaddress_v4();
  return Ipv4Addr(host.toIPv4Address());
}

QHostAddress Ipv4Addr::to_qhostaddress() const { return QHostAddress(this->m_address); }

QString Ipv4Addr::to_qstring() const { return QString::fromStdString(this->to_string()); }

Ipv6Addr Ipv6Addr::from_qstring(QStringView const text)
{
  return parse(text.toString().toStdString());
}

Ipv6Addr Ipv6Addr::from_qstring(QString const& text) { return from_qstring(QStringView {text}); }

Ipv6Addr Ipv6Addr::from_qhostaddress(QHostAddress const& host)
{
  if(host.protocol() != QAbstractSocket::IPv6Protocol)
    throw IpError::invalid_qhostaddress_v6();

  auto octets = std::array<std::uint8_t, 16> {};
  auto const native = host.toIPv6Address();
  std::memcpy(octets.data(), std::data(native.c), octets.size());
  return Ipv6Addr(octets);
}

QHostAddress Ipv6Addr::to_qhostaddress() const
{
  auto native = Q_IPV6ADDR {};
  std::memcpy(std::data(native.c), this->m_octets.data(), this->m_octets.size());
  return QHostAddress(native);
}

QString Ipv6Addr::to_qstring() const { return QString::fromStdString(this->to_string()); }
#endif

WHS31_COMMON_NAMESPACE_END
