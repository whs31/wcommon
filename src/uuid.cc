#include <radar/mms/common/uuid.h>

#include <algorithm>
#include <array>
#include <bit>
#include <limits>
#include <mutex>
#include <random>
#include <vector>

#ifdef RCOMMON_WITH_QT
#  include <qstring.h>
#  include <qstringview.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

// Digest rounds operate on fixed-size blocks with indices defined by MD5/SHA-1.
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access,cppcoreguidelines-pro-bounds-constant-array-index)

namespace
{
  constexpr auto uuid_v7_max_millis = (std::uint64_t {1} << 48U) - 1U;
  constexpr auto uuid_v7_counter_max = (std::uint64_t {1} << 42U) - 1U;
  constexpr auto rfc_namespace_tail = std::array<std::uint8_t, 8> {
    0x80,
    0xb4,
    0x00,
    0xc0,
    0x4f,
    0xd4,
    0x30,
    0xc8,
  };

  struct V7State
  {
    std::uint64_t last_millis = 0;
    std::uint64_t counter = 0;
  };

  struct V7Parts
  {
    std::uint64_t millis;
    std::uint64_t counter;
    std::uint32_t random;
  };

  [[nodiscard]] constexpr std::uint32_t read_le32(
    std::span<std::uint8_t const> const input,
    std::size_t const offset
  ) noexcept
  {
    auto value = std::uint32_t {0};
    for(auto i = std::size_t {0}; i < 4; ++i)
      value |= static_cast<std::uint32_t>(input[offset + i]) << (i * 8U);
    return value;
  }

  [[nodiscard]] constexpr std::uint32_t read_be32(
    std::span<std::uint8_t const> const input,
    std::size_t const offset
  ) noexcept
  {
    auto value = std::uint32_t {0};
    for(auto i = std::size_t {0}; i < 4; ++i)
      value = (value << 8U) | input[offset + i];
    return value;
  }

  void append_length(
    std::vector<std::uint8_t>& input,
    std::uint64_t const bit_length,
    bool const little_endian
  )
  {
    for(auto i = std::size_t {0}; i < 8; ++i) {
      auto const shift = little_endian ? i * 8U : (7U - i) * 8U;
      input.push_back(static_cast<std::uint8_t>(bit_length >> shift));
    }
  }

  [[nodiscard]] std::vector<std::uint8_t> padded(
    std::span<std::uint8_t const> const input,
    bool const little_endian
  )
  {
    auto result = std::vector<std::uint8_t>(input.begin(), input.end());
    auto const bit_length = static_cast<std::uint64_t>(result.size()) * 8U;
    result.push_back(0x80);
    while(result.size() % 64U != 56U)
      result.push_back(0);
    append_length(result, bit_length, little_endian);
    return result;
  }

  [[nodiscard]] std::array<std::uint8_t, 16> md5(std::span<std::uint8_t const> const input)
  {
    constexpr auto shifts = std::array<std::uint32_t, 64> {
      7,  12, 17, 22, 7,  12, 17, 22, 7,  12, 17, 22, 7,  12, 17, 22, 5,  9,  14, 20, 5,  9,
      14, 20, 5,  9,  14, 20, 5,  9,  14, 20, 4,  11, 16, 23, 4,  11, 16, 23, 4,  11, 16, 23,
      4,  11, 16, 23, 6,  10, 15, 21, 6,  10, 15, 21, 6,  10, 15, 21, 6,  10, 15, 21,
    };
    constexpr auto constants = std::array<std::uint32_t, 64> {
      0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613,
      0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193,
      0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d,
      0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
      0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122,
      0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
      0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244,
      0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
      0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb,
      0xeb86d391,
    };

    auto a0 = std::uint32_t {0x67452301};
    auto b0 = std::uint32_t {0xefcdab89};
    auto c0 = std::uint32_t {0x98badcfe};
    auto d0 = std::uint32_t {0x10325476};
    auto const message = padded(input, true);

    for(auto offset = std::size_t {0}; offset < message.size(); offset += 64U) {
      auto words = std::array<std::uint32_t, 16> {};
      for(auto i = std::size_t {0}; i < words.size(); ++i)
        words[i] = read_le32(message, offset + i * 4U);

      auto a = a0;
      auto b = b0;
      auto c = c0;
      auto d = d0;
      for(auto i = std::size_t {0}; i < 64; ++i) {
        auto function = std::uint32_t {0};
        auto word = std::size_t {0};
        if(i < 16) {
          function = (b & c) | (~b & d);
          word = i;
        }
        else if(i < 32) {
          function = (d & b) | (~d & c);
          word = (5U * i + 1U) % 16U;
        }
        else if(i < 48) {
          function = b ^ c ^ d;
          word = (3U * i + 5U) % 16U;
        }
        else {
          function = c ^ (b | ~d);
          word = (7U * i) % 16U;
        }
        auto const next = d;
        d = c;
        c = b;
        b += std::rotl(a + function + constants[i] + words[word], static_cast<int>(shifts[i]));
        a = next;
      }
      a0 += a;
      b0 += b;
      c0 += c;
      d0 += d;
    }

    auto result = std::array<std::uint8_t, 16> {};
    auto const state = std::array {a0, b0, c0, d0};
    for(auto i = std::size_t {0}; i < state.size(); ++i) {
      for(auto j = std::size_t {0}; j < 4; ++j)
        result[i * 4U + j] = static_cast<std::uint8_t>(state[i] >> (j * 8U));
    }
    return result;
  }

  [[nodiscard]] std::array<std::uint8_t, 20> sha1(std::span<std::uint8_t const> const input)
  {
    auto h0 = std::uint32_t {0x67452301};
    auto h1 = std::uint32_t {0xefcdab89};
    auto h2 = std::uint32_t {0x98badcfe};
    auto h3 = std::uint32_t {0x10325476};
    auto h4 = std::uint32_t {0xc3d2e1f0};
    auto const message = padded(input, false);

    for(auto offset = std::size_t {0}; offset < message.size(); offset += 64U) {
      auto words = std::array<std::uint32_t, 80> {};
      for(auto i = std::size_t {0}; i < 16; ++i)
        words[i] = read_be32(message, offset + i * 4U);
      for(auto i = std::size_t {16}; i < words.size(); ++i)
        words[i] = std::rotl(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16], 1);

      auto a = h0;
      auto b = h1;
      auto c = h2;
      auto d = h3;
      auto e = h4;
      for(auto i = std::size_t {0}; i < words.size(); ++i) {
        auto function = std::uint32_t {0};
        auto constant = std::uint32_t {0};
        if(i < 20) {
          function = (b & c) | (~b & d);
          constant = 0x5a827999;
        }
        else if(i < 40) {
          function = b ^ c ^ d;
          constant = 0x6ed9eba1;
        }
        else if(i < 60) {
          function = (b & c) | (b & d) | (c & d);
          constant = 0x8f1bbcdc;
        }
        else {
          function = b ^ c ^ d;
          constant = 0xca62c1d6;
        }
        auto const temporary = std::rotl(a, 5) + function + e + constant + words[i];
        e = d;
        d = c;
        c = std::rotl(b, 30);
        b = a;
        a = temporary;
      }
      h0 += a;
      h1 += b;
      h2 += c;
      h3 += d;
      h4 += e;
    }

    auto result = std::array<std::uint8_t, 20> {};
    auto const state = std::array {h0, h1, h2, h3, h4};
    for(auto i = std::size_t {0}; i < state.size(); ++i) {
      for(auto j = std::size_t {0}; j < 4; ++j)
        result[i * 4U + j] = static_cast<std::uint8_t>(state[i] >> ((3U - j) * 8U));
    }
    return result;
  }

  template <typename Digest>
  [[nodiscard]] Uuid name_uuid(
    Uuid const& namespace_uuid,
    std::span<std::uint8_t const> const name,
    Digest const digest,
    std::uint8_t const version
  )
  {
    auto input = std::vector<std::uint8_t>();
    input.reserve(namespace_uuid.bytes().size() + name.size());
    input.insert(input.end(), namespace_uuid.bytes().begin(), namespace_uuid.bytes().end());
    input.insert(input.end(), name.begin(), name.end());

    auto const hash = digest(input);
    auto uuid = Uuid::from_u64_pair(0, 0);
    std::ranges::copy_n(hash.begin(), 16, uuid.bytes().begin());
    uuid.bytes()[6] = static_cast<std::uint8_t>((uuid.bytes()[6] & 0x0fU) | version);
    uuid.bytes()[8] = static_cast<std::uint8_t>((uuid.bytes()[8] & 0x3fU) | 0x80U);
    return uuid;
  }

  [[nodiscard]] std::mutex& v7_mutex() noexcept
  {
    static auto mutex = std::mutex();
    return mutex;
  }

  [[nodiscard]] V7State& now_v7_state() noexcept
  {
    static auto state = V7State();
    return state;
  }

  [[nodiscard]] V7State& timestamp_v7_state() noexcept
  {
    static auto state = V7State();
    return state;
  }

  [[nodiscard]] std::uint32_t random_u32()
  {
    auto device = std::random_device();
    auto distribution = std::uniform_int_distribution<std::uint32_t> {
      0,
      std::numeric_limits<std::uint32_t>::max(),
    };
    return distribution(device);
  }

  [[nodiscard]] std::uint64_t unix_millis(
    std::chrono::system_clock::time_point const timestamp
  ) noexcept
  {
    auto const millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp.time_since_epoch()
    )
                          .count();
    if(millis <= 0)
      return 0;
    return std::min(static_cast<std::uint64_t>(millis), uuid_v7_max_millis);
  }

  [[nodiscard]] V7Parts next_v7_parts(
    V7State& state,
    std::uint64_t millis,
    bool const clamp_to_last
  )
  {
    auto lock = std::scoped_lock {v7_mutex()};
    if(clamp_to_last and millis < state.last_millis)
      millis = state.last_millis;
    if(millis == state.last_millis) {
      if(state.counter == uuid_v7_counter_max) {
        millis = std::min(millis + 1U, uuid_v7_max_millis);
        state.last_millis = millis;
        state.counter = 0;
      }
      else {
        ++state.counter;
      }
    }
    else {
      state.last_millis = millis;
      state.counter = 0;
    }
    return {
      .millis = state.last_millis,
      .counter = state.counter,
      .random = random_u32(),
    };
  }
}  // namespace

UuidError UuidError::invalid_string(std::string_view const text)
{
  return UuidError("invalid UUID string '{}'", text);
}

Uuid const Uuid::NAMESPACE_DNS = from_fields(0x6ba7b810, 0x9dad, 0x11d1, rfc_namespace_tail);
Uuid const Uuid::NAMESPACE_OID = from_fields(0x6ba7b812, 0x9dad, 0x11d1, rfc_namespace_tail);
Uuid const Uuid::NAMESPACE_URL = from_fields(0x6ba7b811, 0x9dad, 0x11d1, rfc_namespace_tail);
Uuid const Uuid::NAMESPACE_X500 = from_fields(0x6ba7b814, 0x9dad, 0x11d1, rfc_namespace_tail);

Uuid::Uuid(std::span<std::byte const, 16> const bytes) noexcept
{
  for(auto i = std::size_t {0}; i < this->m_bytes.size(); ++i)
    this->m_bytes[i] = std::to_integer<std::uint8_t>(bytes[i]);
}

std::string Uuid::to_string(Format const format) const
{
  auto result = std::string();
  switch(format) {
    case Format::Simple: result.reserve(32); break;
    case Format::Hyphenated: result.reserve(36); break;
    case Format::Braced: result.reserve(38); break;
  }
  if(format == Format::Braced)
    result.push_back('{');
  for(auto i = std::size_t {0}; i < this->m_bytes.size(); ++i) {
    if(format != Format::Simple and (i == 4 or i == 6 or i == 8 or i == 10))
      result.push_back('-');
    result.push_back(encode_hex(this->m_bytes[i] >> 4U));
    result.push_back(encode_hex(this->m_bytes[i] & 0x0fU));
  }
  if(format == Format::Braced)
    result.push_back('}');
  return result;
}

Uuid Uuid::new_v3(Uuid const& namespace_uuid, std::span<std::uint8_t const> const name)
{
  return name_uuid(namespace_uuid, name, md5, 0x30);
}

Uuid Uuid::new_v4()
{
  auto device = std::random_device();
  auto distribution = std::uniform_int_distribution<unsigned int> {0, 0xff};
  auto uuid = Uuid();
  for(auto& byte : uuid.m_bytes)
    byte = static_cast<std::uint8_t>(distribution(device));
  uuid.m_bytes[6] = static_cast<std::uint8_t>((uuid.m_bytes[6] & 0x0fU) | 0x40U);
  uuid.m_bytes[8] = static_cast<std::uint8_t>((uuid.m_bytes[8] & 0x3fU) | 0x80U);
  return uuid;
}

Uuid Uuid::new_v5(Uuid const& namespace_uuid, std::span<std::uint8_t const> const name)
{
  return name_uuid(namespace_uuid, name, sha1, 0x50);
}

Uuid Uuid::new_v7(std::chrono::system_clock::time_point const timestamp)
{
  auto const
    [millis, counter, random] = next_v7_parts(timestamp_v7_state(), unix_millis(timestamp), false);
  return from_u64_pair(
    ((millis & uuid_v7_max_millis) << 16U) | 0x7000U | ((counter >> 30U) & 0x0fffU),
    std::uint64_t {0x8000000000000000} | ((counter & 0x3fffffffU) << 32U) | random
  );
}

Uuid Uuid::now_v7()
{
  auto const
    [millis,
     counter,
     random] = next_v7_parts(now_v7_state(), unix_millis(std::chrono::system_clock::now()), true);
  return from_u64_pair(
    ((millis & uuid_v7_max_millis) << 16U) | 0x7000U | ((counter >> 30U) & 0x0fffU),
    std::uint64_t {0x8000000000000000} | ((counter & 0x3fffffffU) << 32U) | random
  );
}

#ifdef RCOMMON_WITH_QT
QString Uuid::to_qstring(Format const format) const
{
  return QString::fromStdString(this->to_string(format));
}

Uuid Uuid::from_qstring(QStringView const text) { return Uuid(text.toString().toStdString()); }

Uuid Uuid::from_qstring(QString const& text) { return from_qstring(QStringView {text}); }
#endif

int uuid_stream_format_index()
{
  static auto const index = std::ios_base::xalloc();
  return index;
}

std::ostream& operator<<(std::ostream& stream, Uuid::Format const format)
{
  stream.iword(uuid_stream_format_index()) = static_cast<std::int64_t>(format) + 1;
  return stream;
}

std::ostream& operator<<(std::ostream& stream, Uuid const& uuid)
{
  auto const stored = stream.iword(uuid_stream_format_index());
  auto const format = [stored] {
    switch(stored - 1) {
      case static_cast<std::int64_t>(Uuid::Format::Simple): return Uuid::Format::Simple;
      case static_cast<std::int64_t>(Uuid::Format::Braced): return Uuid::Format::Braced;
      default: return Uuid::Format::Hyphenated;
    }
  }();
  return stream << uuid.to_string(format);
}

// NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access,cppcoreguidelines-pro-bounds-constant-array-index)

RADAR_COMMON_NAMESPACE_END
