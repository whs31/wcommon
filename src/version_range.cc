#include <radar/mms/common/version_range.h>

#include <algorithm>
#include <charconv>
#include <compare>
#include <cctype>
#include <limits>
#include <optional>
#include <string>
#include <system_error>

#ifdef RCOMMON_WITH_QT
#  include <qstring.h>
#  include <qstringview.h>
#endif

#include <radar/mms/common/char_utils.h>

RADAR_COMMON_NAMESPACE_BEGIN

namespace
{
  using Comparison = VersionRange::Comparison;
  using Comparator = VersionRange::Comparator;
  using Alternative = VersionRange::Alternative;

  struct PartialVersion
  {
    std::uint64_t major = 0;
    std::uint64_t minor = 0;
    std::uint64_t patch = 0;
    int component_count = 0;
    bool major_wildcard = false;
    bool minor_wildcard = false;
    bool patch_wildcard = false;
    std::string prerelease;
    std::string build;

    [[nodiscard]] bool is_any() const noexcept { return this->major_wildcard; }

    [[nodiscard]] bool has_prerelease() const noexcept { return not this->prerelease.empty(); }

    [[nodiscard]] bool is_full() const noexcept
    {
      return this->component_count == 3 and not this->major_wildcard and not this->minor_wildcard
         and not this->patch_wildcard;
    }
  };

  [[nodiscard]] std::string_view trim(std::string_view text) noexcept
  {
    while(not text.empty() and std::isspace(static_cast<unsigned char>(text.front())) != 0)
      text.remove_prefix(1);
    while(not text.empty() and std::isspace(static_cast<unsigned char>(text.back())) != 0)
      text.remove_suffix(1);
    return text;
  }

  [[nodiscard]] bool is_wildcard(std::string_view const text) noexcept
  {
    return text == "*" or text == "x" or text == "X";
  }

  [[nodiscard]] bool is_operator(std::string_view const text) noexcept
  {
    return text == "=" or text == ">" or text == ">=" or text == "<" or text == "<=";
  }

  [[nodiscard]] std::uint64_t parse_number(
    std::string_view const range,
    std::string_view const number,
    std::string_view const field_name
  )
  {
    if(number.empty())
      throw VersionError::
        invalid_range_string(range, fmt::format("{} component is empty", field_name));
    if(number.size() > 1 and number.front() == '0')
      throw VersionError::
        invalid_range_string(range, fmt::format("{} component has a leading zero", field_name));
    for(auto const c : number) {
      if(not is_digit(c))
        throw VersionError::
          invalid_range_string(range, fmt::format("{} component is not numeric", field_name));
    }

    auto value = std::uint64_t {0};
    auto const* const begin = number.begin();
    auto const* const end = number.end();
    auto const [ptr, ec] = std::from_chars(begin, end, value);
    if(ec == std::errc::result_out_of_range)
      throw VersionError::
        invalid_range_string(range, fmt::format("{} component overflows uint64", field_name));
    if(ec != std::errc {} or ptr != end)
      throw VersionError::
        invalid_range_string(range, fmt::format("{} component is invalid", field_name));
    return value;
  }

  [[nodiscard]] std::uint64_t increment(
    std::uint64_t const value,
    std::string_view const range,
    std::string_view const field_name
  )
  {
    if(value == std::numeric_limits<std::uint64_t>::max())
      throw VersionError::
        invalid_range_string(range, fmt::format("{} component overflows uint64", field_name));
    return value + 1;
  }

  [[nodiscard]] std::vector<std::string_view> split_dot_parts(std::string_view const text)
  {
    auto result = std::vector<std::string_view>();
    auto start = std::size_t {0};
    while(start <= text.size()) {
      auto const dot = text.find('.', start);
      auto const end = dot == std::string_view::npos ? text.size() : dot;
      result.push_back(text.substr(start, end - start));
      if(dot == std::string_view::npos)
        break;
      start = dot + 1;
    }
    return result;
  }

  [[nodiscard]] Version make_version(
    std::uint64_t const major,
    std::uint64_t const minor,
    std::uint64_t const patch,
    std::string_view const prerelease = {},
    std::string_view const build = {}
  )
  {
    return {major, minor, patch, Prerelease(prerelease), BuildMetadata(build)};
  }

  [[nodiscard]] Version lower_bound_version(PartialVersion const& version)
  {
    return make_version(
      version.major_wildcard ? 0 : version.major,
      version.minor_wildcard ? 0 : version.minor,
      version.patch_wildcard ? 0 : version.patch,
      version.prerelease,
      version.build
    );
  }

  [[nodiscard]] Version upper_bound_version(
    PartialVersion const& version,
    std::string_view const range
  )
  {
    if(version.major_wildcard)
      throw VersionError::invalid_range_string(range, "wildcard range has no upper bound");
    if(version.component_count == 1 or version.minor_wildcard)
      return make_version(increment(version.major, range, "major"), 0, 0);
    if(version.component_count == 2 or version.patch_wildcard)
      return make_version(version.major, increment(version.minor, range, "minor"), 0);
    return make_version(version.major, version.minor, increment(version.patch, range, "patch"));
  }

  [[nodiscard]] Version tilde_upper_bound(
    PartialVersion const& version,
    std::string_view const range
  )
  {
    if(version.major_wildcard)
      throw VersionError::invalid_range_string(range, "wildcard range has no tilde upper bound");
    if(version.component_count == 1 or version.minor_wildcard)
      return make_version(increment(version.major, range, "major"), 0, 0);
    return make_version(version.major, increment(version.minor, range, "minor"), 0);
  }

  [[nodiscard]] Version caret_upper_bound(
    PartialVersion const& version,
    std::string_view const range
  )
  {
    if(version.major_wildcard)
      throw VersionError::invalid_range_string(range, "wildcard range has no caret upper bound");
    if(version.major > 0)
      return make_version(increment(version.major, range, "major"), 0, 0);
    if((version.component_count == 1 or version.minor_wildcard) and version.major == 0)
      return make_version(1, 0, 0);
    if(version.minor > 0)
      return make_version(0, increment(version.minor, range, "minor"), 0);
    if(version.component_count == 2 or version.patch_wildcard)
      return make_version(0, 1, 0);
    return make_version(0, 0, increment(version.patch, range, "patch"));
  }

  [[nodiscard]] PartialVersion parse_partial_version(  // NOLINT(readability-function-cognitive-complexity)
    std::string_view const range,
    std::string_view text
  )
  {
    text = trim(text);
    if(text.empty())
      throw VersionError::invalid_range_string(range, "version is empty");

    auto result = PartialVersion();
    if(is_wildcard(text)) {
      result.component_count = 1;
      result.major_wildcard = true;
      result.minor_wildcard = true;
      result.patch_wildcard = true;
      return result;
    }

    auto const plus = text.find('+');
    if(plus != std::string_view::npos and text.find('+', plus + 1) != std::string_view::npos)
      throw VersionError::
        invalid_range_string(range, "build metadata marker appears more than once");
    auto const core_and_pre = text.substr(0, plus);
    if(plus != std::string_view::npos) {
      if(plus + 1 == text.size())
        throw VersionError::invalid_range_string(range, "build metadata is empty");
      result.build = std::string(text.substr(plus + 1));
    }

    auto const hyphen = core_and_pre.find('-');
    auto const core = core_and_pre.substr(0, hyphen);
    if(hyphen != std::string_view::npos) {
      if(hyphen + 1 == core_and_pre.size())
        throw VersionError::invalid_range_string(range, "pre-release identifier is empty");
      result.prerelease = std::string(core_and_pre.substr(hyphen + 1));
    }

    auto const parts = split_dot_parts(core);
    if(parts.empty() or parts.size() > 3)
      throw VersionError::invalid_range_string(range, "version must have one to three components");
    result.component_count = static_cast<int>(parts.size());

    auto const parse_component =
      [&](std::string_view const part, std::string_view const name, bool& wildcard) {
        if(is_wildcard(part)) {
          wildcard = true;
          return std::uint64_t {0};
        }
        return parse_number(range, part, name);
      };

    result.major = parse_component(parts.at(0), "major", result.major_wildcard);
    if(parts.size() >= 2)
      result.minor = parse_component(parts.at(1), "minor", result.minor_wildcard);
    if(parts.size() >= 3)
      result.patch = parse_component(parts.at(2), "patch", result.patch_wildcard);

    if(result.major_wildcard) {
      result.minor_wildcard = true;
      result.patch_wildcard = true;
      if(parts.size() > 1 and not std::ranges::all_of(parts, is_wildcard))
        throw VersionError::
          invalid_range_string(range, "wildcard major cannot have fixed components");
    }
    if(result.minor_wildcard and parts.size() == 3 and not result.patch_wildcard)
      throw VersionError::invalid_range_string(range, "wildcard minor cannot have fixed patch");
    if((not result.prerelease.empty() or not result.build.empty()) and not result.is_full())
      throw VersionError::
        invalid_range_string(range, "pre-release and build metadata require a complete version");

    if(result.is_full()) {
      try {
        static_cast<void>(
          make_version(result.major, result.minor, result.patch, result.prerelease, result.build)
        );
      } catch(std::exception const& e) {
        throw VersionError::invalid_range_string(range, e.what());
      }
    }

    return result;
  }

  void add_comparator(Alternative& alternative, Comparison const comparison, Version version)
  {
    alternative.comparators.push_back(
      Comparator {.comparison = comparison, .version = std::move(version)}
    );
  }

  void add_partial_exact(
    Alternative& alternative,
    PartialVersion const& version,
    std::string_view const range
  )
  {
    if(version.is_any())
      return;
    if(version.is_full()) {
      add_comparator(alternative, Comparison::Equal, lower_bound_version(version));
      return;
    }

    add_comparator(alternative, Comparison::GreaterEqual, lower_bound_version(version));
    add_comparator(alternative, Comparison::Less, upper_bound_version(version, range));
  }

  void add_comparator_expression(
    Alternative& alternative,
    Comparison const comparison,
    PartialVersion const& version,
    std::string_view const range
  )
  {
    if(version.is_any())
      return;

    if(version.is_full()) {
      add_comparator(alternative, comparison, lower_bound_version(version));
      return;
    }

    switch(comparison) {
      case Comparison::Equal: add_partial_exact(alternative, version, range); break;
      case Comparison::Greater:
        add_comparator(alternative, Comparison::GreaterEqual, upper_bound_version(version, range));
        break;
      case Comparison::GreaterEqual:
        add_comparator(alternative, Comparison::GreaterEqual, lower_bound_version(version));
        break;
      case Comparison::Less:
        add_comparator(alternative, Comparison::Less, lower_bound_version(version));
        break;
      case Comparison::LessEqual:
        add_comparator(alternative, Comparison::Less, upper_bound_version(version, range));
        break;
    }
  }

  void add_tilde_expression(
    Alternative& alternative,
    PartialVersion const& version,
    std::string_view const range
  )
  {
    if(version.is_any())
      return;
    add_comparator(alternative, Comparison::GreaterEqual, lower_bound_version(version));
    add_comparator(alternative, Comparison::Less, tilde_upper_bound(version, range));
  }

  void add_caret_expression(
    Alternative& alternative,
    PartialVersion const& version,
    std::string_view const range
  )
  {
    if(version.is_any())
      return;
    add_comparator(alternative, Comparison::GreaterEqual, lower_bound_version(version));
    add_comparator(alternative, Comparison::Less, caret_upper_bound(version, range));
  }

  [[nodiscard]] Comparison parse_comparison(std::string_view const op) noexcept
  {
    if(op == ">")
      return Comparison::Greater;
    if(op == ">=")
      return Comparison::GreaterEqual;
    if(op == "<")
      return Comparison::Less;
    if(op == "<=")
      return Comparison::LessEqual;
    return Comparison::Equal;
  }

  [[nodiscard]] std::vector<std::string_view> split_space_tokens(std::string_view text)
  {
    auto result = std::vector<std::string_view>();
    while(not text.empty()) {
      text = trim(text);
      if(text.empty())
        break;
      auto const end = std::ranges::find_if(
                         text,
                         [](char const c) {
                           return std::isspace(static_cast<unsigned char>(c)) != 0;
                         }
                       )
                     - text.begin();
      result.push_back(text.substr(0, static_cast<std::size_t>(end)));
      text.remove_prefix(static_cast<std::size_t>(end));
    }
    return result;
  }

  [[nodiscard]] std::optional<std::pair<std::string_view, std::string_view>> split_hyphen_range(
    std::string_view const text
  )
  {
    auto const separator = text.find(" - ");
    if(separator == std::string_view::npos)
      return std::nullopt;
    return std::pair {trim(text.substr(0, separator)), trim(text.substr(separator + 3))};
  }

  [[nodiscard]] Alternative parse_alternative(  // NOLINT(readability-function-cognitive-complexity)
    std::string_view const range,
    std::string_view const text
  )
  {
    auto alternative = Alternative();
    auto const trimmed = trim(text);
    if(trimmed.empty())
      throw VersionError::invalid_range_string(range, "empty alternative");

    if(auto const hyphen = split_hyphen_range(trimmed); hyphen.has_value()) {
      auto const lower = parse_partial_version(range, hyphen->first);
      auto const upper = parse_partial_version(range, hyphen->second);
      if(lower.is_any() or upper.is_any())
        throw VersionError::
          invalid_range_string(range, "hyphen range cannot use wildcard endpoint");
      add_comparator(alternative, Comparison::GreaterEqual, lower_bound_version(lower));
      if(upper.is_full())
        add_comparator(alternative, Comparison::LessEqual, lower_bound_version(upper));
      else
        add_comparator(alternative, Comparison::Less, upper_bound_version(upper, range));
      return alternative;
    }

    auto const tokens = split_space_tokens(trimmed);
    for(auto i = std::size_t {0}; i < tokens.size(); ++i) {
      auto token = tokens.at(i);
      auto op = std::string_view();
      auto mode = char {};

      if(token.front() == '~' or token.front() == '^') {
        mode = token.front();
        token.remove_prefix(1);
      }
      else if(is_operator(token)) {
        op = token;
        if(i + 1 >= tokens.size())
          throw VersionError::invalid_range_string(range, "comparator is missing a version");
        token = tokens.at(++i);
      }
      else if(token.starts_with(">=") or token.starts_with("<=")) {
        op = token.substr(0, 2);
        token.remove_prefix(2);
      }
      else if(token.starts_with(">") or token.starts_with("<") or token.starts_with("=")) {
        op = token.substr(0, 1);
        token.remove_prefix(1);
      }

      if(token.empty())
        throw VersionError::invalid_range_string(range, "comparator is missing a version");
      if(token == "-")
        throw VersionError::invalid_range_string(range, "hyphen ranges must use spaced endpoints");

      auto const version = parse_partial_version(range, token);
      if(mode == '~')
        add_tilde_expression(alternative, version, range);
      else if(mode == '^')
        add_caret_expression(alternative, version, range);
      else
        add_comparator_expression(alternative, parse_comparison(op), version, range);
    }

    return alternative;
  }

  [[nodiscard]] std::vector<Alternative> parse_range(std::string_view const expression)
  {
    auto result = std::vector<Alternative>();
    auto rest = trim(expression);
    if(rest.empty())
      rest = "*";

    while(true) {
      auto const separator = rest.find("||");
      auto const alternative = trim(rest.substr(0, separator));
      result.push_back(parse_alternative(expression, alternative));
      if(separator == std::string_view::npos)
        break;
      rest.remove_prefix(separator + 2);
    }

    return result;
  }

  [[nodiscard]] bool same_core(Version const& lhs, Version const& rhs) noexcept
  {
    return lhs.major() == rhs.major() and lhs.minor() == rhs.minor() and lhs.patch() == rhs.patch();
  }

  [[nodiscard]] bool comparator_allows_prerelease(
    Version const& version,
    Comparator const& comparator
  ) noexcept
  {
    return not comparator.version.pre().is_empty() and same_core(version, comparator.version);
  }

  [[nodiscard]] bool compare(Version const& lhs, Comparison const comparison, Version const& rhs)
  {
    auto const result = lhs.cmp_precedence(rhs);
    switch(comparison) {
      case Comparison::Equal: return result == std::strong_ordering::equal;
      case Comparison::Greater: return result == std::strong_ordering::greater;
      case Comparison::GreaterEqual: return result != std::strong_ordering::less;
      case Comparison::Less: return result == std::strong_ordering::less;
      case Comparison::LessEqual: return result != std::strong_ordering::greater;
    }
    return false;
  }

  [[nodiscard]] bool contains(
    Alternative const& alternative,
    Version const& version,
    bool const include_prerelease
  )
  {
    if(not include_prerelease and not version.pre().is_empty()) {
      auto const allowed = std::ranges::any_of(alternative.comparators, [&](Comparator const& c) {
        return comparator_allows_prerelease(version, c);
      });
      if(not allowed)
        return false;
    }

    return std::ranges::all_of(alternative.comparators, [&](Comparator const& comparator) {
      return compare(version, comparator.comparison, comparator.version);
    });
  }
}  // namespace

VersionRange::VersionRange()
  : m_expression("*")
  , m_alternatives(parse_range("*"))
{}

VersionRange::VersionRange(std::string_view const expression)
  : m_expression(std::string(trim(expression.empty() ? std::string_view {"*"} : expression)))
  , m_alternatives(parse_range(this->m_expression))
{}

bool VersionRange::contains(Version const& version) const noexcept
{
  return std::ranges::any_of(this->m_alternatives, [&](Alternative const& alternative) {
    return ::radar::mms::common::contains(alternative, version, this->m_include_prerelease);
  });
}

bool VersionRange::is_any() const noexcept
{
  return this->m_alternatives.size() == 1 and this->m_alternatives.front().comparators.empty();
}

bool VersionRange::is_any_with_prerelease() const noexcept
{
  return this->is_any() and this->m_include_prerelease;
}

VersionRange VersionRange::any() { return {}; }

VersionRange VersionRange::any_with_prerelease()
{
  auto range = VersionRange();
  range.m_include_prerelease = true;
  return range;
}

VersionRange VersionRange::parse(std::string_view const expression)
{
  return VersionRange(expression);
}

#ifdef RCOMMON_WITH_QT
QString VersionRange::to_qstring() const { return QString::fromStdString(this->to_string()); }

VersionRange VersionRange::from_qstring(QStringView const text)
{
  return VersionRange(text.toString().toStdString());
}

VersionRange VersionRange::from_qstring(QString const& text)
{
  return from_qstring(QStringView {text});
}
#endif

std::ostream& operator<<(std::ostream& stream, VersionRange const& range)
{
  return stream << range.to_string();
}

RADAR_COMMON_NAMESPACE_END
