#include <whs31/common/sockaddr.h>

#include <charconv>
#include <ostream>
#include <system_error>

#include <fmt/format.h>

WHS31_COMMON_NAMESPACE_BEGIN

namespace
{
  [[nodiscard]] std::uint16_t parse_port(std::string_view const text)
  {
    auto port = std::uint32_t {};
    auto const [position, error] = std::from_chars(text.begin(), text.end(), port);
    if(error != std::errc {} or position != text.end() or port > 65'535)
      throw IpError::invalid_port(text);
    return static_cast<std::uint16_t>(port);
  }
}  // namespace

SockAddrV4 SockAddrV4::parse(std::string_view const text)
{
  auto const colon = text.rfind(':');
  if(colon == std::string_view::npos)
    throw IpError::missing_colon(text);
  return {Ipv4Addr::parse(text.substr(0, colon)), parse_port(text.substr(colon + 1))};
}

std::string SockAddrV4::to_string() const { return fmt::format("{}:{}", this->m_ip, this->m_port); }

SockAddrV6 SockAddrV6::parse(std::string_view const text)
{
  if(text.empty() or text.front() != '[')
    throw IpError::invalid_v6_string(text);

  auto const close = text.find(']');
  if(close == std::string_view::npos or close + 1 >= text.size() or text.at(close + 1) != ':')
    throw IpError::invalid_v6_string(text);

  return {Ipv6Addr::parse(text.substr(1, close - 1)), parse_port(text.substr(close + 2))};
}

std::string SockAddrV6::to_string() const
{
  return fmt::format("[{}]:{}", this->m_ip, this->m_port);
}

SockAddr SockAddr::parse(std::string_view const text)
{
  if(not text.empty() and text.front() == '[')
    return SockAddr(SockAddrV6::parse(text));
  return SockAddr(SockAddrV4::parse(text));
}

bool SockAddr::is_ipv4() const noexcept { return std::holds_alternative<SockAddrV4>(m_address); }

bool SockAddr::is_ipv6() const noexcept { return std::holds_alternative<SockAddrV6>(m_address); }

SockAddr::Family SockAddr::family() const noexcept
{
  return this->is_ipv4() ? Family::Ipv4 : Family::Ipv6;
}

std::uint32_t SockAddr::port() const noexcept
{
  if(auto const* address = std::get_if<SockAddrV4>(&this->m_address))
    return address->port();
  if(auto const* address = std::get_if<SockAddrV6>(&this->m_address))
    return address->port();
  return 0;
}

SockAddrV4 const& SockAddr::as_v4() const
{
  if(auto const* address = std::get_if<SockAddrV4>(&this->m_address))
    return *address;
  throw IpError::not_a_v4();
}

SockAddrV6 const& SockAddr::as_v6() const
{
  if(auto const* address = std::get_if<SockAddrV6>(&this->m_address))
    return *address;
  throw IpError::not_a_v6();
}

std::string SockAddr::to_string() const
{
  return std::visit([](auto const& address) { return address.to_string(); }, this->m_address);
}

bool SockAddr::operator==(SockAddr const& other) const noexcept
{
  if(auto const* left = std::get_if<SockAddrV4>(&this->m_address)) {
    auto const* right = std::get_if<SockAddrV4>(&other.m_address);
    return right != nullptr and *left == *right;
  }
  auto const* left = std::get_if<SockAddrV6>(&this->m_address);
  auto const* right = std::get_if<SockAddrV6>(&other.m_address);
  return left != nullptr and right != nullptr and *left == *right;
}

std::strong_ordering SockAddr::operator<=>(SockAddr const& other) const noexcept
{
  if(this->family() != other.family())
    return this->family() == Family::Ipv4 ? std::strong_ordering::less
                                          : std::strong_ordering::greater;
  if(auto const* left = std::get_if<SockAddrV4>(&this->m_address)) {
    auto const* right = std::get_if<SockAddrV4>(&other.m_address);
    return right == nullptr ? std::strong_ordering::equal : *left <=> *right;
  }
  auto const* left = std::get_if<SockAddrV6>(&this->m_address);
  auto const* right = std::get_if<SockAddrV6>(&other.m_address);
  if(left != nullptr and right != nullptr)
    return *left <=> *right;
  return std::strong_ordering::equal;
}

#ifdef WHS31_COMMON_WITH_QT
SockAddrV4 SockAddrV4::from_qstring(QStringView const text)
{
  return parse(text.toString().toStdString());
}

SockAddrV4 SockAddrV4::from_qstring(QString const& text)
{
  return from_qstring(QStringView {text});
}

QString SockAddrV4::to_qstring() const { return QString::fromStdString(this->to_string()); }

SockAddrV6 SockAddrV6::from_qstring(QStringView const text)
{
  return parse(text.toString().toStdString());
}

SockAddrV6 SockAddrV6::from_qstring(QString const& text)
{
  return from_qstring(QStringView {text});
}

QString SockAddrV6::to_qstring() const { return QString::fromStdString(this->to_string()); }

SockAddr SockAddr::from_qstring(QStringView const text)
{
  return parse(text.toString().toStdString());
}

SockAddr SockAddr::from_qstring(QString const& text) { return from_qstring(QStringView {text}); }

QString SockAddr::to_qstring() const { return QString::fromStdString(this->to_string()); }
#endif

std::ostream& operator<<(std::ostream& stream, SockAddrV4 const& address)
{
  return stream << address.to_string();
}

std::ostream& operator<<(std::ostream& stream, SockAddrV6 const& address)
{
  return stream << address.to_string();
}

std::ostream& operator<<(std::ostream& stream, SockAddr const& address)
{
  return stream << address.to_string();
}

WHS31_COMMON_NAMESPACE_END
