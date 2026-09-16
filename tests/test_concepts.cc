#include <radar/mms/common/concepts.h>

#include <memory>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("Concepts classify numbers and pointers", "[concepts]")
{
  STATIC_CHECK(Number<int>);
  STATIC_CHECK(Number<int const&>);
  STATIC_CHECK(Number<double>);
  STATIC_CHECK(not Number<bool>);
  STATIC_CHECK(not Number<std::nullptr_t>);

  STATIC_CHECK(Pointer<int*>);
  STATIC_CHECK(Pointer<void (*)()>);
  STATIC_CHECK(not Pointer<int&>);
  STATIC_CHECK(SmartPointer<std::unique_ptr<int>>);
  STATIC_CHECK(SmartPointer<std::shared_ptr<int>>);
  STATIC_CHECK(PointerLike<int*>);
  STATIC_CHECK(PointerLike<std::unique_ptr<int>>);
  STATIC_CHECK(not PointerLike<int>);
}
