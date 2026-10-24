#include <whs31/common/glaze/frequency.h>
#include <whs31/common/glaze/read.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include <glaze/json.hpp>
#include <glaze/toml.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace whs31;
using Catch::Matchers::ContainsSubstring;
using Catch::Matchers::MessageMatches;
using Catch::Matchers::StartsWith;

// glaze reflects on the names of the members, which clang rejects for a type without linkage, so
// these stay out of the anonymous namespace
namespace document_glaze_test
{
  struct DocumentLink  // NOLINT(misc-use-internal-linkage)
  {
    Frequency carrier;
    std::optional<std::string> name;

    [[nodiscard]] bool operator==(DocumentLink const&) const = default;
  };

  struct DocumentOptions  // NOLINT(misc-use-internal-linkage)
  {
    std::optional<int> retries;
  };
}  // namespace document_glaze_test

using document_glaze_test::DocumentLink;
using document_glaze_test::DocumentOptions;

TEST_CASE("Glaze documents are read strictly", "[glaze][document]")
{
  CHECK(
    read_glaze<DocumentLink>(R"({"carrier":{"hertz":2400}})")
    == DocumentLink {.carrier = Frequency::from_hertz(2'400.0)}
  );
  CHECK(
    read_glaze<DocumentLink, strict_toml>("carrier = \"2400hz\"\nname = \"uplink\"")
    == DocumentLink {.carrier = Frequency::from_hertz(2'400.0), .name = "uplink"}
  );
  // an empty document is an empty table
  CHECK_FALSE(read_glaze<DocumentOptions, strict_toml>("").retries.has_value());

  CHECK_THROWS_AS(read_glaze<DocumentLink>(R"({"name":"uplink"})"), DocumentError);
  CHECK_THROWS_AS(
    read_glaze<DocumentLink>(R"({"carrier":{"hertz":2400},"power":1})"),
    DocumentError
  );
  CHECK_THROWS_WITH(
    (read_glaze<DocumentLink, strict_toml>("carrier = \"2400hz\"\nname = 7\n")),
    ContainsSubstring("name = 7")
  );
}

TEST_CASE("Glaze document files name the file in their errors", "[glaze][document]")
{
  auto const directory = std::filesystem::temp_directory_path() / "wcommon-glaze-documents";
  std::filesystem::create_directories(directory);
  auto const valid = directory / "valid.toml";
  auto const invalid = directory / "invalid.toml";
  std::ofstream(valid) << "carrier = { kilohertz = 2.4 }\n";
  std::ofstream(invalid) << "carrier = \"2.4 parsecs\"\n";

  CHECK(
    read_glaze_file<DocumentLink, strict_toml>(valid).carrier == Frequency::from_kilohertz(2.4)
  );
  CHECK_THROWS_MATCHES(
    (read_glaze_file<DocumentLink, strict_toml>(invalid)),
    DocumentError,
    MessageMatches(StartsWith("'" + invalid.string() + "': "))
  );
  CHECK_THROWS_MATCHES(
    (read_glaze_file<DocumentLink, strict_toml>(directory / "missing.toml")),
    DocumentError,
    MessageMatches(StartsWith("cannot read '") and ContainsSubstring("missing.toml"))
  );

  std::filesystem::remove_all(directory);
}
