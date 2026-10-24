#include <whs31/common/json/uuid.h>

#include <nlohmann/json.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

TEST_CASE("Uuid JSON uses the canonical string representation", "[uuid][json]")
{
  auto const uuid = Uuid("00112233-4455-6677-8899-aabbccddeeff");
  auto const json = nlohmann::json(uuid);

  CHECK(json == "00112233-4455-6677-8899-aabbccddeeff");
  CHECK(json.get<Uuid>() == uuid);
  CHECK_THROWS(nlohmann::json(42).get<Uuid>());
  CHECK_THROWS_AS(nlohmann::json("00112233445566778899aabbccddeeff").get<Uuid>(), UuidError);
}
