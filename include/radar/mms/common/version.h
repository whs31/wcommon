#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/error.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#  include <qstringview.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

class RADAR_COMMON_API VersionError final : public Error
{
 public:
  template <typename... Args>
  explicit VersionError(fmt::format_string<Args...> fmt, Args&&... args)
    : Error(fmt, std::forward<Args>(args)...)
  {}

  [[nodiscard]] static VersionError invalid_string(std::string_view text, std::string_view reason);
  [[nodiscard]] static VersionError invalid_prerelease_identifier(std::string_view text);
  [[nodiscard]] static VersionError invalid_build_metadata(std::string_view text);
  [[nodiscard]] static VersionError invalid_range_string(
    std::string_view text,
    std::string_view reason
  );
};

class RADAR_COMMON_API Prerelease
{
 public:
  static Prerelease const EMPTY;

  Prerelease() noexcept = default;
  explicit Prerelease(std::string_view text);

  [[nodiscard]] std::string const& as_str() const noexcept { return this->m_value; }

  [[nodiscard]] bool is_empty() const noexcept { return this->m_value.empty(); }

  [[nodiscard]] bool operator==(Prerelease const&) const noexcept = default;
  [[nodiscard]] std::strong_ordering operator<=>(Prerelease const& other) const noexcept;

 private:
  std::string m_value;
};

class RADAR_COMMON_API BuildMetadata
{
 public:
  static BuildMetadata const EMPTY;

  BuildMetadata() noexcept = default;
  explicit BuildMetadata(std::string_view text);

  [[nodiscard]] std::string const& as_str() const noexcept { return this->m_value; }

  [[nodiscard]] bool is_empty() const noexcept { return this->m_value.empty(); }

  [[nodiscard]] bool operator==(BuildMetadata const&) const noexcept = default;
  [[nodiscard]] std::strong_ordering operator<=>(BuildMetadata const& other) const noexcept;

 private:
  std::string m_value;
};

/**
 * @brief Semantic version value type.
 * @note Qt properties, QString conversions, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 * @ingroup core_identifiers
 */
class RADAR_COMMON_API Version
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET
  Q_PROPERTY(std::uint64_t major READ major CONSTANT FINAL)
  Q_PROPERTY(std::uint64_t minor READ minor CONSTANT FINAL)
  Q_PROPERTY(std::uint64_t patch READ patch CONSTANT FINAL)
  Q_PROPERTY(QString prerelease READ prerelease_qstring CONSTANT FINAL)
  Q_PROPERTY(QString buildMetadata READ build_metadata_qstring CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  Version() noexcept = default;

  constexpr Version(
    std::uint64_t const major,
    std::uint64_t const minor,
    std::uint64_t const patch
  ) noexcept
    : m_major(major)
    , m_minor(minor)
    , m_patch(patch)
  {}

  Version(
    std::uint64_t major,
    std::uint64_t minor,
    std::uint64_t patch,
    Prerelease pre,
    BuildMetadata build = BuildMetadata()
  ) noexcept;

  explicit Version(std::string_view text);

  [[nodiscard]] constexpr std::uint64_t major() const noexcept { return this->m_major; }

  [[nodiscard]] constexpr std::uint64_t minor() const noexcept { return this->m_minor; }

  [[nodiscard]] constexpr std::uint64_t patch() const noexcept { return this->m_patch; }

  [[nodiscard]] Prerelease const& pre() const noexcept { return this->m_pre; }

  [[nodiscard]] BuildMetadata const& build() const noexcept { return this->m_build; }

  [[nodiscard]] std::string to_string() const;

  [[nodiscard]] static constexpr Version new_version(
    std::uint64_t const major,
    std::uint64_t const minor,
    std::uint64_t const patch
  ) noexcept
  {
    return {major, minor, patch};
  }

  [[nodiscard]] static Version parse(std::string_view text);

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString prerelease_qstring() const;

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString build_metadata_qstring() const;

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Version from_qstring(QStringView text);

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static Version from_qstring(QString const& text);
#endif

  [[nodiscard]] std::strong_ordering cmp_precedence(Version const& other) const noexcept;
  [[nodiscard]] bool operator==(Version const&) const noexcept = default;
  [[nodiscard]] std::strong_ordering operator<=>(Version const& other) const noexcept;

 private:
  std::uint64_t m_major = 0;
  std::uint64_t m_minor = 0;
  std::uint64_t m_patch = 0;
  Prerelease m_pre;
  BuildMetadata m_build;
};

[[nodiscard]] RADAR_COMMON_API Version operator""_version(char const* text, std::size_t length);

RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, Prerelease const& value);
RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, BuildMetadata const& value);
RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, Version const& value);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::Prerelease>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Prerelease const& value) const noexcept
  {
    return std::hash<std::string> {}(value.as_str());
  }
};

template <>
struct std::hash<radar::mms::common::BuildMetadata>
{
  [[nodiscard]] std::size_t operator()(
    radar::mms::common::BuildMetadata const& value
  ) const noexcept
  {
    return std::hash<std::string> {}(value.as_str());
  }
};

template <>
struct std::hash<radar::mms::common::Version>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::Version const& version) const noexcept
  {
    auto hash = std::hash<std::uint64_t> {}(version.major());
    auto const combine = [&hash](std::size_t const value) {
      hash ^= value + 0x9e3779b97f4a7c15ull + (hash << 6U) + (hash >> 2U);
    };
    combine(std::hash<std::uint64_t> {}(version.minor()));
    combine(std::hash<std::uint64_t> {}(version.patch()));
    combine(std::hash<radar::mms::common::Prerelease> {}(version.pre()));
    combine(std::hash<radar::mms::common::BuildMetadata> {}(version.build()));
    return hash;
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::Prerelease> : fmt::ostream_formatter
{};

template <>
struct fmt::formatter<radar::mms::common::BuildMetadata> : fmt::ostream_formatter
{};

template <>
struct fmt::formatter<radar::mms::common::Version> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Prerelease)
Q_DECLARE_METATYPE(radar::mms::common::BuildMetadata)
Q_DECLARE_METATYPE(radar::mms::common::Version)
#endif
