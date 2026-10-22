#pragma once

#include <cstddef>
#include <cstdint>

#include <array>
#include <bit>
#include <concepts>

#include <radar/mms/common/bit.h>
#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief The results of a pure function for the arguments it was asked for last, `Size` of them,
 * each in the one place the hash of its arguments picks: a newer key of the same place takes it
 * over.
 *
 * Keys are the bit patterns of the arguments (bits_of()), so a hit returns exactly what the
 * function would, signed zeros included. Not synchronized: keep one per thread, e.g. for a lookup
 * that several systems of a step ask for the same point.
 *
 * @tparam Words 64-bit words of a key.
 * @tparam Size Places, a power of two above one.
 */
template <std::size_t Words, typename Value, std::size_t Size>
  requires(
    Words > 0 and Size > 1 and std::has_single_bit(Size) and std::default_initializable<Value>
  )
class Memo
{
 public:
  using Key = std::array<std::uint64_t, Words>;

  /**
   * @brief The value kept for `key`, null if its place holds another key or none.
   */
  [[nodiscard]] Value const* find(Key const& key) const noexcept
  {
    // place_of() masks the place below Size
    // NOLINTNEXTLINE(*-constant-array-index,*-avoid-unchecked-container-access)
    auto const& entry = this->m_entries[place_of(key)];
    return entry.used and entry.key == key ? &entry.value : nullptr;
  }

  /**
   * @brief Keeps `value` for `key`, in place of whatever its place held; the value kept.
   */
  Value const& store(Key const& key, Value const& value) noexcept
  {
    // place_of() masks the place below Size
    // NOLINTNEXTLINE(*-constant-array-index,*-avoid-unchecked-container-access)
    auto& entry = this->m_entries[place_of(key)];
    entry.key = key;
    entry.value = value;
    entry.used = true;
    return entry.value;
  }

 private:
  struct Entry
  {
    Key key = {};
    Value value = {};
    bool used = false;
  };

  [[nodiscard]] static std::size_t place_of(Key const& key) noexcept
  {
    auto constexpr multiplier = std::uint64_t {0x9e37'79b9'7f4a'7c15};
    auto hash = std::uint64_t {0};
    for(auto const word : key)
      hash = hash ^ word * multiplier;
    return static_cast<std::size_t>(
             hash >> (64 - std::countr_zero(Size))
           )  // NOLINT(*-signed-bitwise)
         & (Size - 1);
  }

  std::array<Entry, Size> m_entries = {};
};

RADAR_COMMON_NAMESPACE_END
