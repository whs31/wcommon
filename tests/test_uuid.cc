#include <radar/mms/common/uuid.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

namespace
{
  auto constexpr uuid_text = "00112233-4455-6677-8899-aAbBcCdDeEfF";
  auto constexpr uuid_bytes = std::array<std::uint8_t, 16> {
    0x00,
    0x11,
    0x22,
    0x33,
    0x44,
    0x55,
    0x66,
    0x77,
    0x88,
    0x99,
    0xaa,
    0xbb,
    0xcc,
    0xdd,
    0xee,
    0xff,
  };
  auto constexpr field_bytes = std::array<std::uint8_t, 8> {
    0x88,
    0x99,
    0xaa,
    0xbb,
    0xcc,
    0xdd,
    0xee,
    0xff,
  };

  [[nodiscard]] std::span<std::uint8_t const> bytes(std::string_view const text) noexcept
  {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): chars may be viewed as bytes
    return {reinterpret_cast<std::uint8_t const*>(text.data()), text.size()};
  }
}  // namespace

TEST_CASE("Uuid supports constexpr construction and inspection", "[uuid]")
{
  auto constexpr uuid = Uuid(uuid_text);
  auto constexpr little = Uuid::from_fields_le(0x00112233, 0x4455, 0x6677, field_bytes);
  auto constexpr custom = Uuid::new_v8(uuid_bytes);

  STATIC_CHECK(Uuid::SHORT_STRING_LEN == 36);
  STATIC_CHECK(Uuid::LONG_STRING_LEN == 38);
  STATIC_CHECK(Uuid() == Uuid::nil());
  STATIC_CHECK(std::ranges::equal(uuid.bytes(), uuid_bytes));
  STATIC_CHECK(uuid.version() == Uuid::Version::SortMac);
  STATIC_CHECK(Uuid::from_fields(0x00112233, 0x4455, 0x6677, field_bytes) == uuid);
  STATIC_CHECK(Uuid::from_u64_pair(0x0011223344556677, 0x8899aabbccddeeff) == uuid);
  STATIC_CHECK(little == Uuid("33221100-5544-7766-8899-aabbccddeeff"));
  STATIC_CHECK(custom.version() == Uuid::Version::Custom);
  STATIC_CHECK(Uuid::max().is_max());
  STATIC_CHECK("00112233-4455-6677-8899-aabbccddeeff"_uuid == uuid);
  STATIC_CHECK_FALSE(noexcept(Uuid::new_v3(Uuid::NAMESPACE_DNS, std::span<std::uint8_t const> {})));
  STATIC_CHECK_FALSE(noexcept(Uuid::new_v5(Uuid::NAMESPACE_DNS, std::span<std::uint8_t const> {})));
}

TEST_CASE("Uuid parses and formats every supported representation", "[uuid]")
{
  auto const uuid = Uuid(uuid_text);

  CHECK(uuid == Uuid("{00112233-4455-6677-8899-aabbccddeeff}"));
  CHECK(uuid.to_string(Uuid::Format::Simple) == "00112233445566778899aabbccddeeff");
  CHECK(uuid.to_string() == "00112233-4455-6677-8899-aabbccddeeff");
  CHECK(uuid.to_string(Uuid::Format::Braced) == "{00112233-4455-6677-8899-aabbccddeeff}");

  auto const [d1, d2, d3, d4] = uuid.as_fields();
  CHECK(d1 == 0x00112233);
  CHECK(d2 == 0x4455);
  CHECK(d3 == 0x6677);
  CHECK(std::ranges::equal(d4, field_bytes));
}

TEST_CASE("Uuid rejects malformed strings even when their length is valid", "[uuid]")
{
  for(auto const text : {
        std::string_view {"001122334455-6677-8899-aabbccddeeff"},
        std::string_view {"00112233-4455-6677-8899-aabbccddeezz"},
        std::string_view {"[00112233-4455-6677-8899-aabbccddeeff]"},
        std::string_view {"{00112233-4455-6677-8899-aabbccddeeff)"},
        std::string_view {"00112233-4455-6677-8899-aabbccddeef"},
        std::string_view {"00112233-4455-6677-8899-aabbccddeeff\0", 37},
      }) {
    CAPTURE(text);
    CHECK_THROWS_AS(Uuid(text), UuidError);
  }
}

TEST_CASE("Uuid namespace generators match RFC vectors", "[uuid]")
{
  CHECK(Uuid::NAMESPACE_DNS == Uuid("6ba7b810-9dad-11d1-80b4-00c04fd430c8"));
  CHECK(Uuid::NAMESPACE_URL == Uuid("6ba7b811-9dad-11d1-80b4-00c04fd430c8"));
  CHECK(Uuid::NAMESPACE_OID == Uuid("6ba7b812-9dad-11d1-80b4-00c04fd430c8"));
  CHECK(Uuid::NAMESPACE_X500 == Uuid("6ba7b814-9dad-11d1-80b4-00c04fd430c8"));
  CHECK(
    Uuid::new_v3(Uuid::NAMESPACE_DNS, bytes("rust-lang.org"))
    == Uuid("c6db027c-615c-3b4d-959e-1a917747ca5a")
  );
  CHECK(
    Uuid::new_v5(Uuid::NAMESPACE_DNS, bytes("rust-lang.org"))
    == Uuid("c66bbb60-d62e-5f17-a399-3a0bd237c503")
  );

  auto const long_name = std::string(100, 'a');
  CHECK(
    Uuid::new_v3(Uuid::NAMESPACE_DNS, bytes(long_name))
    == Uuid("9e765498-aa7f-338a-9c78-08f217cd658f")
  );
  CHECK(
    Uuid::new_v5(Uuid::NAMESPACE_DNS, bytes(long_name))
    == Uuid("56596f37-716c-57a9-a735-2561f8608390")
  );
}

TEST_CASE("Uuid random and time generators set RFC bits", "[uuid]")
{
  auto const random = Uuid::new_v4();
  auto const timestamp = std::chrono::system_clock::time_point {
    std::chrono::milliseconds {1'497'624'119'000}
  };
  auto const first = Uuid::new_v7(timestamp);
  auto const second = Uuid::new_v7(timestamp);

  CHECK(random.version() == Uuid::Version::Random);
  CHECK((random.bytes().subspan<8>().front() & 0xc0U) == 0x80U);
  CHECK(first.version() == Uuid::Version::SortRand);
  CHECK(first < second);
  CHECK(Uuid::now_v7().version() == Uuid::Version::SortRand);
}

TEST_CASE("Uuid supports streams fmt ordering and hashing", "[uuid]")
{
  auto const uuid = Uuid(uuid_text);
  auto stream = std::ostringstream();
  stream << Uuid::Format::Braced << uuid;

  CHECK(stream.str() == "{00112233-4455-6677-8899-aabbccddeeff}");
  CHECK(fmt::format("{:s}", uuid) == "00112233445566778899aabbccddeeff");
  CHECK(Uuid::nil() < uuid);
  CHECK(std::unordered_set<Uuid> {uuid, uuid, Uuid::nil()}.size() == 2);
}

TEST_CASE("Uuid supports byte construction and mutation", "[uuid]")
{
  auto constexpr storage = std::array<std::byte, 16> {
    std::byte {0x00},
    std::byte {0x11},
    std::byte {0x22},
    std::byte {0x33},
    std::byte {0x44},
    std::byte {0x55},
    std::byte {0x66},
    std::byte {0x77},
    std::byte {0x88},
    std::byte {0x99},
    std::byte {0xaa},
    std::byte {0xbb},
    std::byte {0xcc},
    std::byte {0xdd},
    std::byte {0xee},
    std::byte {0xff},
  };
  auto uuid = Uuid(storage);
  CHECK(std::ranges::equal(uuid.bytes(), uuid_bytes));

  uuid.bytes().front() = 0xff;
  CHECK(uuid != Uuid(uuid_text));
}
