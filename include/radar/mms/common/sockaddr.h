#pragma once

#include <compare>
#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <string_view>
#include <variant>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/ip.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#  include <qstringview.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief IPv4 socket address.
 * @note Qt properties, QString conversions, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 * @ingroup core_networking
 */
class RADAR_COMMON_API SockAddrV4
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET
  Q_PROPERTY(Ipv4Addr ip READ ip CONSTANT FINAL)
  Q_PROPERTY(std::uint32_t port READ port CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  constexpr SockAddrV4() noexcept = default;

  constexpr SockAddrV4(Ipv4Addr const ip, std::uint16_t const port) noexcept
    : m_ip(ip)
    , m_port(port)
  {}

  [[nodiscard]] static constexpr SockAddrV4 from_parts(
    Ipv4Addr const ip,
    std::uint16_t const port
  ) noexcept
  {
    return {ip, port};
  }

  [[nodiscard]] static SockAddrV4 parse(std::string_view text);

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static SockAddrV4 from_qstring(QStringView text);

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static SockAddrV4 from_qstring(QString const& text);
#endif

  [[nodiscard]] constexpr Ipv4Addr ip() const noexcept { return this->m_ip; }

  [[nodiscard]] constexpr std::uint32_t port() const noexcept { return this->m_port; }

  [[nodiscard]] std::string to_string() const;

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] constexpr bool operator==(SockAddrV4 const&) const noexcept = default;
  [[nodiscard]] constexpr std::strong_ordering operator<=>(SockAddrV4 const&) const noexcept =
    default;

 private:
  Ipv4Addr m_ip;
  std::uint16_t m_port = 0;
};

/**
 * @brief IPv6 socket address.
 * @note Qt properties, QString conversions, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 * @ingroup core_networking
 */
class RADAR_COMMON_API SockAddrV6
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET
  Q_PROPERTY(Ipv6Addr ip READ ip CONSTANT FINAL)
  Q_PROPERTY(std::uint32_t port READ port CONSTANT FINAL)
  Q_PROPERTY(std::uint32_t flowInfo READ flow_info CONSTANT FINAL)
  Q_PROPERTY(std::uint32_t scopeId READ scope_id CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  constexpr SockAddrV6() noexcept = default;

  constexpr SockAddrV6(
    Ipv6Addr const ip,
    std::uint16_t const port,
    std::uint32_t const flow_info = 0,
    std::uint32_t const scope_id = 0
  ) noexcept
    : m_ip(ip)
    , m_port(port)
    , m_flow_info(flow_info)
    , m_scope_id(scope_id)
  {}

  [[nodiscard]] static constexpr SockAddrV6 from_parts(
    Ipv6Addr const ip,
    std::uint16_t const port,
    std::uint32_t const flow_info = 0,
    std::uint32_t const scope_id = 0
  ) noexcept
  {
    return {ip, port, flow_info, scope_id};
  }

  [[nodiscard]] static SockAddrV6 parse(std::string_view text);

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static SockAddrV6 from_qstring(QStringView text);

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static SockAddrV6 from_qstring(QString const& text);
#endif

  [[nodiscard]] constexpr Ipv6Addr ip() const noexcept { return this->m_ip; }

  [[nodiscard]] constexpr std::uint32_t port() const noexcept { return this->m_port; }

  [[nodiscard]] constexpr std::uint32_t flow_info() const noexcept { return this->m_flow_info; }

  [[nodiscard]] constexpr std::uint32_t scope_id() const noexcept { return this->m_scope_id; }

  [[nodiscard]] std::string to_string() const;

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] constexpr bool operator==(SockAddrV6 const&) const noexcept = default;
  [[nodiscard]] constexpr std::strong_ordering operator<=>(SockAddrV6 const&) const noexcept =
    default;

 private:
  Ipv6Addr m_ip;
  std::uint16_t m_port = 0;
  std::uint32_t m_flow_info = 0;
  std::uint32_t m_scope_id = 0;
};

/**
 * @brief Type-erased IPv4 or IPv6 socket address.
 * @note Qt properties, QString conversions, enum reflection, and metatype
 * registration are available only when rcommon is built with `with_qt=True`.
 * @ingroup core_networking
 */
class RADAR_COMMON_API SockAddr
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET
  Q_PROPERTY(Family family READ family CONSTANT FINAL)
  Q_PROPERTY(bool isIpv4 READ is_ipv4 CONSTANT FINAL)
  Q_PROPERTY(bool isIpv6 READ is_ipv6 CONSTANT FINAL)
  Q_PROPERTY(std::uint32_t port READ port CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  enum class Family : std::uint8_t
  {
    Ipv4,
    Ipv6,
  };
#ifdef RCOMMON_WITH_QT
  Q_ENUM(Family)
#endif

  SockAddr() noexcept = default;

  explicit SockAddr(SockAddrV4 const& address) noexcept
    : m_address(address)
  {}

  explicit SockAddr(SockAddrV6 const& address) noexcept
    : m_address(address)
  {}

  [[nodiscard]] static SockAddr parse(std::string_view text);

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static SockAddr from_qstring(QStringView text);

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static SockAddr from_qstring(QString const& text);
#endif

  [[nodiscard]] bool is_ipv4() const noexcept;

  [[nodiscard]] bool is_ipv6() const noexcept;

  [[nodiscard]] Family family() const noexcept;

  [[nodiscard]] std::uint32_t port() const noexcept;

  [[nodiscard]] SockAddrV4 const& as_v4() const;

  [[nodiscard]] SockAddrV6 const& as_v6() const;

  [[nodiscard]] std::string to_string() const;

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] bool operator==(SockAddr const& other) const noexcept;
  [[nodiscard]] std::strong_ordering operator<=>(SockAddr const& other) const noexcept;

 private:
  std::variant<SockAddrV4, SockAddrV6> m_address = SockAddrV4();
};

RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, SockAddrV4 const& address);
RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, SockAddrV6 const& address);
RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, SockAddr const& address);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::SockAddrV4>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::SockAddrV4 const& address) const noexcept
  {
    auto hash = std::hash<radar::mms::common::Ipv4Addr> {}(address.ip());
    hash ^= std::hash<std::uint32_t> {}(address.port()) + 0x9e3779b97f4a7c15ULL + (hash << 6)
          + (hash >> 2);
    return hash;
  }
};

template <>
struct std::hash<radar::mms::common::SockAddrV6>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::SockAddrV6 const& address) const noexcept
  {
    auto hash = std::hash<radar::mms::common::Ipv6Addr> {}(address.ip());
    for(auto const value : {address.port(), address.flow_info(), address.scope_id()}) {
      hash ^= std::hash<std::uint32_t> {}(value) + 0x9e3779b97f4a7c15ULL + (hash << 6)
            + (hash >> 2);
    }
    return hash;
  }
};

template <>
struct std::hash<radar::mms::common::SockAddr>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::SockAddr const& address) const
  {
    return address.is_ipv4() ? std::hash<radar::mms::common::SockAddrV4> {}(address.as_v4())
                             : std::hash<radar::mms::common::SockAddrV6> {}(address.as_v6());
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::SockAddrV4> : fmt::ostream_formatter
{};

template <>
struct fmt::formatter<radar::mms::common::SockAddrV6> : fmt::ostream_formatter
{};

template <>
struct fmt::formatter<radar::mms::common::SockAddr> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::SockAddrV4)
Q_DECLARE_METATYPE(radar::mms::common::SockAddrV6)
Q_DECLARE_METATYPE(radar::mms::common::SockAddr)
#endif
