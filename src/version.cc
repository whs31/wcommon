#include <radar/mms/common/version.h>

#include <algorithm>
#include <charconv>
#include <system_error>
#include <utility>

#ifdef RCOMMON_WITH_QT
#  include <qstring.h>
#  include <qstringview.h>
#endif

#include <radar/mms/common/char_utils.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace
{
  [[nodiscard]] bool is_identifier_char(char const value) noexcept
  {
    return is_digit(value) or is_letter(value) or value == '-';
  }

  [[nodiscard]] bool is_numeric_identifier(std::string_view const identifier) noexcept
  {
    return not identifier.empty()
       and std::ranges::all_of(identifier, [](char const value) { return is_digit(value); });
  }

  [[nodiscard]] bool validate_identifier_list(
    std::string_view const text,
    bool const forbid_numeric_leading_zero
  ) noexcept
  {
    if(text.empty())
      return true;

    auto start = std::size_t {0};
    while(start <= text.size()) {
      auto const dot = text.find('.', start);
      auto const end = dot == std::string_view::npos ? text.size() : dot;
      auto const identifier = text.substr(start, end - start);
      if(identifier.empty() or not std::ranges::all_of(identifier, [](char const value) {
           return is_identifier_char(value);
         }))
        return false;
      if(
        forbid_numeric_leading_zero and identifier.size() > 1 and identifier.front() == '0'
        and is_numeric_identifier(identifier)
      )
        return false;
      if(dot == std::string_view::npos)
        break;
      start = dot + 1;
    }
    return true;
  }

  [[nodiscard]] std::string_view strip_numeric_zeros(std::string_view value) noexcept
  {
    while(value.size() > 1 and value.front() == '0')
      value.remove_prefix(1);
    return value;
  }

  [[nodiscard]] std::strong_ordering compare_identifier_numbers(
    std::string_view const left,
    std::string_view const right
  ) noexcept
  {
    auto const lhs = strip_numeric_zeros(left);
    auto const rhs = strip_numeric_zeros(right);
    if(lhs.size() != rhs.size())
      return lhs.size() <=> rhs.size();
    if(auto const result = lhs <=> rhs; result != std::strong_ordering::equal)
      return result;
    return left.size() <=> right.size();
  }

  [[nodiscard]] std::strong_ordering compare_identifier_lists(
    std::string_view const left,
    std::string_view const right
  ) noexcept
  {
    auto left_start = std::size_t {0};
    auto right_start = std::size_t {0};
    while(left_start < left.size() or right_start < right.size()) {
      if(left_start >= left.size())
        return std::strong_ordering::less;
      if(right_start >= right.size())
        return std::strong_ordering::greater;

      auto const left_dot = left.find('.', left_start);
      auto const right_dot = right.find('.', right_start);
      auto const left_end = left_dot == std::string_view::npos ? left.size() : left_dot;
      auto const right_end = right_dot == std::string_view::npos ? right.size() : right_dot;
      auto const lhs = left.substr(left_start, left_end - left_start);
      auto const rhs = right.substr(right_start, right_end - right_start);
      auto const lhs_numeric = is_numeric_identifier(lhs);
      auto const rhs_numeric = is_numeric_identifier(rhs);

      if(lhs_numeric and rhs_numeric) {
        if(
          auto const result = compare_identifier_numbers(lhs, rhs);
          result != std::strong_ordering::equal
        )
          return result;
      }
      else if(lhs_numeric != rhs_numeric) {
        return lhs_numeric ? std::strong_ordering::less : std::strong_ordering::greater;
      }
      else if(auto const result = lhs <=> rhs; result != std::strong_ordering::equal) {
        return result;
      }

      left_start = left_dot == std::string_view::npos ? left.size() : left_dot + 1;
      right_start = right_dot == std::string_view::npos ? right.size() : right_dot + 1;
    }
    return std::strong_ordering::equal;
  }

  [[nodiscard]] std::uint64_t parse_core_number(
    std::string_view const version,
    std::string_view const number,
    std::string_view const field
  )
  {
    if(number.empty())
      throw VersionError::invalid_string(version, fmt::format("{} component is empty", field));
    if(number.size() > 1 and number.front() == '0')
      throw VersionError::
        invalid_string(version, fmt::format("{} component has a leading zero", field));
    if(not std::ranges::all_of(number, [](char const value) { return is_digit(value); }))
      throw VersionError::
        invalid_string(version, fmt::format("{} component is not numeric", field));

    auto result = std::uint64_t {0};
    auto const [position, error] = std::from_chars(number.begin(), number.end(), result);
    if(error == std::errc::result_out_of_range)
      throw VersionError::
        invalid_string(version, fmt::format("{} component overflows uint64", field));
    if(error != std::errc {} or position != number.end())
      throw VersionError::invalid_string(version, fmt::format("{} component is invalid", field));
    return result;
  }
}  // namespace

VersionError VersionError::invalid_string(
  std::string_view const text,
  std::string_view const reason
)
{
  return VersionError("invalid version string '{}': {}", text, reason);
}

VersionError VersionError::invalid_prerelease_identifier(std::string_view const text)
{
  return VersionError("invalid semantic version pre-release identifier '{}'", text);
}

VersionError VersionError::invalid_build_metadata(std::string_view const text)
{
  return VersionError("invalid semantic version build metadata '{}'", text);
}

VersionError VersionError::invalid_range_string(
  std::string_view const text,
  std::string_view const reason
)
{
  return VersionError("invalid semantic version range '{}': {}", text, reason);
}

Prerelease const Prerelease::EMPTY = Prerelease();
BuildMetadata const BuildMetadata::EMPTY = BuildMetadata();

Prerelease::Prerelease(std::string_view const text)
{
  if(not validate_identifier_list(text, true))
    throw VersionError::invalid_prerelease_identifier(text);
  this->m_value = text;
}

std::strong_ordering Prerelease::operator<=>(Prerelease const& other) const noexcept
{
  return compare_identifier_lists(this->m_value, other.m_value);
}

BuildMetadata::BuildMetadata(std::string_view const text)
{
  if(not validate_identifier_list(text, false))
    throw VersionError::invalid_build_metadata(text);
  this->m_value = text;
}

std::strong_ordering BuildMetadata::operator<=>(BuildMetadata const& other) const noexcept
{
  return compare_identifier_lists(this->m_value, other.m_value);
}

Version::Version(
  std::uint64_t const major,
  std::uint64_t const minor,
  std::uint64_t const patch,
  Prerelease pre,
  BuildMetadata build
) noexcept
  : m_major(major)
  , m_minor(minor)
  , m_patch(patch)
  , m_pre(std::move(pre))
  , m_build(std::move(build))
{}

Version::Version(std::string_view const text)
  : Version(parse(text))
{}

std::string Version::to_string() const
{
  auto result = fmt::format("{}.{}.{}", this->m_major, this->m_minor, this->m_patch);
  if(not this->m_pre.is_empty())
    result += fmt::format("-{}", this->m_pre);
  if(not this->m_build.is_empty())
    result += fmt::format("+{}", this->m_build);
  return result;
}

Version Version::parse(std::string_view const text)
{
  auto const plus = text.find('+');
  if(plus != std::string_view::npos and text.find('+', plus + 1) != std::string_view::npos)
    throw VersionError::invalid_string(text, "build metadata marker appears more than once");

  auto const core_and_pre = text.substr(0, plus);
  auto const build = plus == std::string_view::npos ? std::string_view {} : text.substr(plus + 1);
  if(plus != std::string_view::npos and build.empty())
    throw VersionError::invalid_string(text, "build metadata is empty");
  if(not validate_identifier_list(build, false))
    throw VersionError::invalid_string(text, "build metadata is invalid");

  auto const hyphen = core_and_pre.find('-');
  auto const core = core_and_pre.substr(0, hyphen);
  auto const pre = hyphen == std::string_view::npos ? std::string_view {}
                                                    : core_and_pre.substr(hyphen + 1);
  if(hyphen != std::string_view::npos and pre.empty())
    throw VersionError::invalid_string(text, "pre-release identifier is empty");
  if(not validate_identifier_list(pre, true))
    throw VersionError::invalid_string(text, "pre-release identifier is invalid");

  auto const first_dot = core.find('.');
  auto const second_dot = first_dot == std::string_view::npos ? std::string_view::npos
                                                              : core.find('.', first_dot + 1);
  if(
    first_dot == std::string_view::npos or second_dot == std::string_view::npos
    or core.find('.', second_dot + 1) != std::string_view::npos
  )
    throw VersionError::invalid_string(text, "version must have exactly three numeric components");

  return {
    parse_core_number(text, core.substr(0, first_dot), "major"),
    parse_core_number(text, core.substr(first_dot + 1, second_dot - first_dot - 1), "minor"),
    parse_core_number(text, core.substr(second_dot + 1), "patch"),
    Prerelease(pre),
    BuildMetadata(build)
  };
}

std::strong_ordering Version::cmp_precedence(Version const& other) const noexcept
{
  if(auto const result = this->m_major <=> other.m_major; result != std::strong_ordering::equal)
    return result;
  if(auto const result = this->m_minor <=> other.m_minor; result != std::strong_ordering::equal)
    return result;
  if(auto const result = this->m_patch <=> other.m_patch; result != std::strong_ordering::equal)
    return result;
  if(this->m_pre.is_empty() != other.m_pre.is_empty())
    return this->m_pre.is_empty() ? std::strong_ordering::greater : std::strong_ordering::less;
  return this->m_pre <=> other.m_pre;
}

std::strong_ordering Version::operator<=>(Version const& other) const noexcept
{
  if(auto const result = this->cmp_precedence(other); result != std::strong_ordering::equal)
    return result;
  return this->m_build <=> other.m_build;
}

Version operator""_version(char const* const text, std::size_t const length)
{
  return Version(std::string_view {text, length});
}

#ifdef RCOMMON_WITH_QT
QString Version::prerelease_qstring() const { return QString::fromStdString(this->m_pre.as_str()); }

QString Version::build_metadata_qstring() const
{
  return QString::fromStdString(this->m_build.as_str());
}

QString Version::to_qstring() const { return QString::fromStdString(this->to_string()); }

Version Version::from_qstring(QStringView const text)
{
  return Version(text.toString().toStdString());
}

Version Version::from_qstring(QString const& text) { return from_qstring(QStringView {text}); }
#endif

std::ostream& operator<<(std::ostream& stream, Prerelease const& value)
{
  return stream << value.as_str();
}

std::ostream& operator<<(std::ostream& stream, BuildMetadata const& value)
{
  return stream << value.as_str();
}

std::ostream& operator<<(std::ostream& stream, Version const& value)
{
  return stream << value.to_string();
}

RADAR_COMMON_NAMESPACE_END
