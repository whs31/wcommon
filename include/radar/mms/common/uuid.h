#pragma once

#include <array>
#include <bit>
#include <chrono>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ios>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include <fmt/format.h>

#include <radar/mms/common/char_utils.h>
#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/error.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#  include <qstringview.h>
#  include <quuid.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

class RADAR_COMMON_API UuidError final : public Error
{
 public:
  template <typename... Args>
  explicit UuidError(fmt::format_string<Args...> fmt, Args&&... args)
    : Error(fmt, std::forward<Args>(args)...)
  {}

  [[nodiscard]] static UuidError invalid_string(std::string_view text);
};

/**
 * @brief Universally unique identifier value type.
 * @note Qt properties, conversions, and metatype registration are available
 * only when rcommon is built with `with_qt=True`.
 * @ingroup core_identifiers
 */
class RADAR_COMMON_API Uuid
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
  Q_PROPERTY(std::uint64_t versionNum READ version_num CONSTANT FINAL)
  Q_PROPERTY(bool isNil READ is_nil CONSTANT FINAL)
  Q_PROPERTY(bool isMax READ is_max CONSTANT FINAL)
#endif

 public:
  enum class Version : std::uint8_t
  {
    Nil = 0,
    Mac = 1,
    Dce = 2,
    Md5 = 3,
    Random = 4,
    Sha1 = 5,
    SortMac = 6,
    SortRand = 7,
    Custom = 8,
    Max = 15,
    None = 255,
  };

  enum class Format : std::uint8_t
  {
    Simple,
    Hyphenated,
    Braced,
  };

  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr static std::size_t SHORT_STRING_LEN = 36;
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr static std::size_t LONG_STRING_LEN = 38;

  static Uuid const NAMESPACE_DNS;   // NOLINT(readability-identifier-naming)
  static Uuid const NAMESPACE_OID;   // NOLINT(readability-identifier-naming)
  static Uuid const NAMESPACE_URL;   // NOLINT(readability-identifier-naming)
  static Uuid const NAMESPACE_X500;  // NOLINT(readability-identifier-naming)

  constexpr Uuid() noexcept = default;

  explicit Uuid(std::span<std::byte const, 16> bytes) noexcept;

  explicit constexpr Uuid(unsigned char const (&bytes)[16]) noexcept  // NOLINT(*-avoid-c-arrays)
  {
    for(auto i = std::size_t {0}; i < this->m_bytes.size(); ++i)
      this->m_bytes.at(i) = static_cast<std::uint8_t>(bytes[i]);  // NOLINT(*-constant-array-index)
  }

  explicit constexpr Uuid(std::string_view const text)
  {
    auto const braced = text.size() == LONG_STRING_LEN;
    if(text.size() != SHORT_STRING_LEN and not braced)
      throw UuidError::invalid_string(text);
    if(braced and (text.front() != '{' or text.back() != '}'))
      throw UuidError::invalid_string(text);

    auto const value = braced ? text.substr(1, SHORT_STRING_LEN) : text;
    if(value.at(8) != '-' or value.at(13) != '-' or value.at(18) != '-' or value.at(23) != '-')
      throw UuidError::invalid_string(text);

    auto constexpr offsets = std::array<std::size_t, 16> {
      0,
      2,
      4,
      6,
      9,
      11,
      14,
      16,
      19,
      21,
      24,
      26,
      28,
      30,
      32,
      34,
    };
    for(auto i = std::size_t {0}; i < offsets.size(); ++i) {
      auto const offset = offsets.at(i);
      if(not is_hex(value.at(offset)) or not is_hex(value.at(offset + 1)))
        throw UuidError::invalid_string(text);
      this->m_bytes.at(i) = hex_byte_join(value, static_cast<int>(offset));
    }
  }

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  // QUuid exposes its final field as a fixed-size C array.
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
  explicit constexpr Uuid(QUuid const& uuid) noexcept
    : m_bytes {
        byte<3>(uuid.data1),
        byte<2>(uuid.data1),
        byte<1>(uuid.data1),
        byte<0>(uuid.data1),
        byte<1>(uuid.data2),
        byte<0>(uuid.data2),
        byte<1>(uuid.data3),
        byte<0>(uuid.data3),
        static_cast<std::uint8_t>(uuid.data4[0]),
        static_cast<std::uint8_t>(uuid.data4[1]),
        static_cast<std::uint8_t>(uuid.data4[2]),
        static_cast<std::uint8_t>(uuid.data4[3]),
        static_cast<std::uint8_t>(uuid.data4[4]),
        static_cast<std::uint8_t>(uuid.data4[5]),
        static_cast<std::uint8_t>(uuid.data4[6]),
        static_cast<std::uint8_t>(uuid.data4[7]),
      }
  {}

  // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
#endif

  [[nodiscard]] constexpr bool is_nil() const noexcept { return *this == nil(); }

  [[nodiscard]] constexpr bool is_max() const noexcept { return *this == max(); }

  [[nodiscard]] constexpr std::span<std::uint8_t const, 16> bytes() const noexcept
  {
    return this->m_bytes;
  }

  [[nodiscard]] constexpr std::span<std::uint8_t, 16> bytes() noexcept { return this->m_bytes; }

  [[nodiscard]] std::string to_string(Format format = Format::Hyphenated) const;

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring(Format format = Format::Hyphenated) const;

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Uuid from_qstring(QStringView text);

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Uuid from_qstring(QString const& text);

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] constexpr QUuid to_quuid() const noexcept
  {
    auto const [d1, d2, d3, d4] = this->as_fields();
    // std::span has no bounds-checked element access before C++26; the extent is fixed at 8.
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    return {
      static_cast<uint>(d1),
      static_cast<ushort>(d2),
      static_cast<ushort>(d3),
      static_cast<uchar>(d4[0]),
      static_cast<uchar>(d4[1]),
      static_cast<uchar>(d4[2]),
      static_cast<uchar>(d4[3]),
      static_cast<uchar>(d4[4]),
      static_cast<uchar>(d4[5]),
      static_cast<uchar>(d4[6]),
      static_cast<uchar>(d4[7]),
    };
    // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
  }
#endif

  [[nodiscard]] constexpr static Uuid nil() noexcept { return {}; }

  [[nodiscard]] constexpr static Uuid max() noexcept
  {
    auto uuid = Uuid();
    uuid.m_bytes.fill(0xff);
    return uuid;
  }

  [[nodiscard]] static Uuid new_v3(Uuid const& namespace_uuid, std::span<std::uint8_t const> name);
  [[nodiscard]] static Uuid new_v4();
  [[nodiscard]] static Uuid new_v5(Uuid const& namespace_uuid, std::span<std::uint8_t const> name);
  [[nodiscard]] static Uuid new_v7(std::chrono::system_clock::time_point timestamp);
  [[nodiscard]] static Uuid now_v7();

  [[nodiscard]] constexpr static Uuid new_v8(std::array<std::uint8_t, 16> const bytes) noexcept
  {
    auto uuid = Uuid();
    uuid.m_bytes = bytes;
    uuid.m_bytes.at(6) = static_cast<std::uint8_t>((uuid.m_bytes.at(6) & 0x0fU) | 0x80U);
    uuid.m_bytes.at(8) = static_cast<std::uint8_t>((uuid.m_bytes.at(8) & 0x3fU) | 0x80U);
    return uuid;
  }

  [[nodiscard]] constexpr static Uuid from_fields(
    std::uint32_t const d1,
    std::uint16_t const d2,
    std::uint16_t const d3,
    std::span<std::uint8_t const, 8> const d4
  ) noexcept
  {
    return from_parts(d1, d2, d3, d4, false);
  }

  [[nodiscard]] constexpr static Uuid from_fields_le(
    std::uint32_t const d1,
    std::uint16_t const d2,
    std::uint16_t const d3,
    std::span<std::uint8_t const, 8> const d4
  ) noexcept
  {
    return from_parts(d1, d2, d3, d4, true);
  }

  [[nodiscard]] constexpr static Uuid from_u64_pair(
    std::uint64_t const high_bits,
    std::uint64_t const low_bits
  ) noexcept
  {
    auto uuid = Uuid();
    for(auto i = std::size_t {0}; i < 8; ++i) {
      uuid.m_bytes.at(i) = static_cast<std::uint8_t>(high_bits >> ((7 - i) * 8));
      uuid.m_bytes.at(i + 8) = static_cast<std::uint8_t>(low_bits >> ((7 - i) * 8));
    }
    return uuid;
  }

  [[nodiscard]] constexpr std::
    tuple<std::uint32_t, std::uint16_t, std::uint16_t, std::span<std::uint8_t const, 8>>
    as_fields() const noexcept
  {
    return {
      this->read_be<std::uint32_t, 0>(),
      this->read_be<std::uint16_t, 4>(),
      this->read_be<std::uint16_t, 6>(),
      std::span<std::uint8_t const, 16> {this->m_bytes}.template subspan<8, 8>(),
    };
  }

  [[nodiscard]] constexpr std::size_t version_num() const noexcept
  {
    return static_cast<std::size_t>((this->m_bytes.at(6) >> 4U) & 0x0fU);
  }

  [[nodiscard]] constexpr Version version() const noexcept
  {
    if(this->is_nil())
      return Version::Nil;
    if(this->is_max())
      return Version::Max;
    switch(this->version_num()) {
      case 1: return Version::Mac;
      case 2: return Version::Dce;
      case 3: return Version::Md5;
      case 4: return Version::Random;
      case 5: return Version::Sha1;
      case 6: return Version::SortMac;
      case 7: return Version::SortRand;
      case 8: return Version::Custom;
      default: return Version::None;
    }
  }

  [[nodiscard]] constexpr bool operator==(Uuid const&) const noexcept = default;
  [[nodiscard]] constexpr std::strong_ordering operator<=>(Uuid const&) const noexcept = default;

 private:
  template <std::size_t ByteIndex, typename Num>
  [[nodiscard]] constexpr static std::uint8_t byte(Num const value) noexcept
  {
    return static_cast<std::uint8_t>(value >> (ByteIndex * 8));
  }

  [[nodiscard]] constexpr static Uuid from_parts(
    std::uint32_t const d1,
    std::uint16_t const d2,
    std::uint16_t const d3,
    std::span<std::uint8_t const, 8> const d4,
    bool const little_endian
  ) noexcept
  {
    auto uuid = Uuid();
    // std::span has no bounds-checked element access before C++26; the extent is fixed at 8.
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    uuid.m_bytes = {
      little_endian ? byte<0>(d1) : byte<3>(d1),
      little_endian ? byte<1>(d1) : byte<2>(d1),
      little_endian ? byte<2>(d1) : byte<1>(d1),
      little_endian ? byte<3>(d1) : byte<0>(d1),
      little_endian ? byte<0>(d2) : byte<1>(d2),
      little_endian ? byte<1>(d2) : byte<0>(d2),
      little_endian ? byte<0>(d3) : byte<1>(d3),
      little_endian ? byte<1>(d3) : byte<0>(d3),
      d4[0],
      d4[1],
      d4[2],
      d4[3],
      d4[4],
      d4[5],
      d4[6],
      d4[7],
    };
    // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    return uuid;
  }

  template <typename Num, std::size_t Offset>
  [[nodiscard]] constexpr Num read_be() const noexcept
  {
    auto value = Num {0};
    for(auto i = std::size_t {0}; i < sizeof(Num); ++i)
      value = static_cast<Num>((value << 8U) | this->m_bytes.at(Offset + i));
    return value;
  }

  std::array<std::uint8_t, 16> m_bytes {};
};

[[nodiscard]] constexpr Uuid operator""_uuid(char const* const text, std::size_t const length)
{
  return Uuid(std::string_view {text, length});
}

[[nodiscard]] RADAR_COMMON_API int uuid_stream_format_index();
RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, Uuid::Format format);
RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, Uuid const& uuid);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Uuid>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Uuid const& uuid) const noexcept
  {
    auto hash = static_cast<std::size_t>(14'695'981'039'346'656'037ull);
    for(auto const byte : uuid.bytes()) {
      hash ^= static_cast<std::size_t>(byte);
      hash *= static_cast<std::size_t>(1'099'511'628'211ull);
    }
    return hash;
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::Uuid>
{
  radar::mms::common::Uuid::Format output_format = radar::mms::common::Uuid::Format::Hyphenated;

  auto constexpr parse(fmt::format_parse_context& context)
  {
    // fmt exposes its parse position as an iterator; advancing it is the required API.
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic,readability-qualified-auto)
    auto iterator = context.begin();
    if(iterator == context.end() or *iterator == '}')
      return iterator;
    switch(*iterator) {
      case 's': output_format = radar::mms::common::Uuid::Format::Simple; break;
      case 'h': output_format = radar::mms::common::Uuid::Format::Hyphenated; break;
      case 'b': output_format = radar::mms::common::Uuid::Format::Braced; break;
      default: throw fmt::format_error("invalid UUID format specifier");
    }
    ++iterator;
    if(iterator != context.end() and *iterator != '}')
      throw fmt::format_error("invalid UUID format specifier");
    return iterator;
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic,readability-qualified-auto)
  }

  template <typename FormatContext>
  auto format(radar::mms::common::Uuid const& uuid, FormatContext& context) const
  {
    return fmt::format_to(context.out(), "{}", uuid.to_string(output_format));
  }
};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Uuid)
#endif
