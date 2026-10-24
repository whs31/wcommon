#include <whs31/common/error.h>
#include <whs31/common/memory/null_pointer_error.h>

#include <cerrno>
#include <exception>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

#include <catch2/catch_test_macros.hpp>

using namespace whs31;

TEST_CASE("Errors retain formatted diagnostics", "[error]")
{
  auto const error = Error("value {}", 42);
  REQUIRE(std::string_view(error.what()) == "value 42");

  auto const null_error = NullPointerError("widget");
  REQUIRE(std::string_view(null_error.what()) == "null pointer dereference (widget)");

  auto const system_error = SystemError(std::system_error(EINVAL, std::generic_category()));
  auto const prefix = "system error " + std::to_string(EINVAL) + ": ";
  REQUIRE(std::string_view(system_error.what()).starts_with(prefix));
}

TEST_CASE("NotImplementedError has balanced location formatting", "[error]")
{
  auto const error = NotImplementedError();
  auto const message = std::string_view(error.what());

  REQUIRE(message.starts_with("not implemented (file "));
  REQUIRE(message.ends_with(")"));
}

TEST_CASE("Exception pointers describe their exception", "[error]")
{
  REQUIRE(describe(std::make_exception_ptr(NumericError("singular matrix"))) == "singular matrix");
  REQUIRE(describe(std::make_exception_ptr(std::runtime_error("disk full"))) == "disk full");
  REQUIRE(describe(std::make_exception_ptr(42)) == "unknown exception");
  REQUIRE(describe(std::exception_ptr()).empty());
}
