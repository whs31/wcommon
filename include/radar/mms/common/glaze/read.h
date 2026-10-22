#pragma once

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <system_error>

#include <glaze/core/opts.hpp>
#include <glaze/core/read.hpp>
#include <glaze/core/reflect.hpp>

#include <radar/mms/common/check.h>
#include <radar/mms/common/error.h>
#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Error thrown when a document cannot be read or does not describe the type read from it.
 * @details The message is glaze's, with the line of the document at fault, prefixed by the file
 * name for read_glaze_file().
 * @ingroup core_glaze
 * @ingroup core_errors
 */
class DocumentError final : public Error
{
 public:
  using Error::Error;
};

/**
 * @brief Glaze options of a strict JSON read: unknown and missing keys are errors.
 * @details A member that is a `std::optional`, or that the type's `glz::meta` excuses through
 * `requires_key`, may still be missing.
 * @ingroup core_glaze
 */
inline auto constexpr strict_json = glz::opts {
  .error_on_unknown_keys = true,
  .error_on_missing_keys = true,
};

/**
 * @brief Glaze options of a strict TOML read, e.g. of a configuration file: unknown and missing
 * keys are errors.
 * @ingroup core_glaze
 * @see strict_json
 */
inline auto constexpr strict_toml = glz::opts {
  .format = glz::TOML,
  .error_on_unknown_keys = true,
  .error_on_missing_keys = true,
};

/**
 * @brief Reads @p document into a `T`.
 * @details The glaze headers of the format, e.g. `<glaze/toml.hpp>`, and the adapters of the types
 * read must be included.
 *
 * @code {.cpp}
 * struct Link
 * {
 *   radar::mms::Frequency carrier;
 *   std::optional<std::string> name;
 * };
 *
 * auto const link = radar::mms::read_glaze<Link, radar::mms::strict_toml>(R"(carrier = "2.4ghz")");
 * @endcode
 *
 * @tparam T Type to read, default-constructible.
 * @tparam Opts Glaze options, the format among them.
 * @param document The document, with or without a final line break.
 * @return The value the document describes.
 * @throws DocumentError If the document is malformed or does not describe a `T`; the message
 * shows the offending line.
 * @ingroup core_glaze
 */
template <typename T, auto Opts = strict_json>
[[nodiscard]] T read_glaze(std::string_view const document)
{
  // note: [whs31 : 2026-10-22]
  // glaze rejects an empty buffer, e.g. an empty TOML document, and shows no line for an error at
  // the very end of one
  auto buffer = std::string(document);
  if(not buffer.ends_with('\n'))
    buffer.push_back('\n');
  auto value = T {};
  auto const error = glz::read<Opts>(value, buffer);
  RADAR_HARD_CHECK(not error, DocumentError, "{}", glz::format_error(error, buffer));
  return value;
}

/**
 * @brief Reads the file at @p path into a `T`.
 * @tparam T Type to read, default-constructible.
 * @tparam Opts Glaze options, the format among them.
 * @param path File to read.
 * @return The value the file describes.
 * @throws DocumentError If the file cannot be read, is malformed or does not describe a `T`; the
 * message names the file.
 * @ingroup core_glaze
 * @see read_glaze()
 */
template <typename T, auto Opts = strict_json>
[[nodiscard]] T read_glaze_file(std::filesystem::path const& path)
{
  auto file = std::ifstream(path, std::ios::binary);
  if(not file.is_open()) {
    auto ec = std::error_code {};
    auto const absolute = std::filesystem::absolute(path, ec);
    throw DocumentError("cannot read '{}'", (ec ? path : absolute).string());
  }
  auto const document = std::string(std::istreambuf_iterator<char>(file), {});
  try {
    return read_glaze<T, Opts>(document);
  } catch(DocumentError const& error) {
    throw DocumentError("'{}': {}", path.string(), error.what());
  }
}

RADAR_COMMON_NAMESPACE_END
