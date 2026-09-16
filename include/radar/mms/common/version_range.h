#pragma once

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/ostream.h>

#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/version.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#  include <qstringview.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Semantic version requirement expression.
 * @note Qt properties, QString conversions, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 * @ingroup core_identifiers
 */
class RADAR_COMMON_API VersionRange
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
  Q_PROPERTY(bool any READ is_any CONSTANT FINAL)
  Q_PROPERTY(bool any_with_prerelease READ is_any_with_prerelease CONSTANT FINAL)
#endif

 public:
  VersionRange();
  explicit VersionRange(std::string_view expression);

#ifdef RCOMMON_WITH_QT
  Q_INVOKABLE
#endif
  [[nodiscard]] bool contains(Version const& version) const noexcept;
#ifdef RCOMMON_WITH_QT
  Q_INVOKABLE
#endif
  [[nodiscard]] bool satisfies(Version const& version) const noexcept
  {
    return this->contains(version);
  }

  [[nodiscard]] bool is_any() const noexcept;
  [[nodiscard]] bool is_any_with_prerelease() const noexcept;

  [[nodiscard]] std::string const& to_string() const noexcept { return this->m_expression; }

  [[nodiscard]] static VersionRange any();
  [[nodiscard]] static VersionRange any_with_prerelease();
  [[nodiscard]] static VersionRange parse(std::string_view expression);

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static VersionRange from_qstring(QStringView text);

  /// @note Available only when rcommon is built with `with_qt=True`.
  [[nodiscard]] static VersionRange from_qstring(QString const& text);
#endif

  [[nodiscard]] bool operator==(VersionRange const&) const noexcept = default;

  enum class Comparison : std::uint8_t
  {
    Equal,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
  };

  struct Comparator
  {
    Comparison comparison = Comparison::Equal;
    Version version;
    [[nodiscard]] bool operator==(Comparator const&) const noexcept = default;
  };

  struct Alternative
  {
    std::vector<Comparator> comparators;
    [[nodiscard]] bool operator==(Alternative const&) const noexcept = default;
  };

 private:
  std::string m_expression = "*";
  std::vector<Alternative> m_alternatives;
  bool m_include_prerelease = false;
};

RADAR_COMMON_API std::ostream& operator<<(std::ostream& stream, VersionRange const& range);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::VersionRange>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::VersionRange const& range) const noexcept
  {
    auto const expression = std::hash<std::string> {}(range.to_string());
    auto const prerelease = std::hash<bool> {}(range.is_any_with_prerelease());
    return expression ^ (prerelease + 0x9e3779b9U + (expression << 6U) + (expression >> 2U));
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::VersionRange> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::VersionRange)
#endif
