#include <radar/mms/common/char_utils.h>

#include <cstdint>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("ASCII character helpers classify their inputs", "[char_utils]")
{
  STATIC_CHECK(to_lower('A') == 'a');
  STATIC_CHECK(to_lower('z') == 'z');
  STATIC_CHECK(to_lower('7') == '7');

  STATIC_CHECK(is_digit('0'));
  STATIC_CHECK(is_digit('9'));
  STATIC_CHECK(not is_digit('/'));
  STATIC_CHECK(not is_digit(':'));
  STATIC_CHECK(is_space(' '));
  STATIC_CHECK(not is_space('\t'));
  STATIC_CHECK(is_operator('<'));
  STATIC_CHECK(is_operator('>'));
  STATIC_CHECK(is_operator('='));
  STATIC_CHECK(not is_operator('!'));
  STATIC_CHECK(is_dot('.'));
  STATIC_CHECK(is_logical_or('|'));
  STATIC_CHECK(is_hyphen('-'));
  STATIC_CHECK(is_letter('A'));
  STATIC_CHECK(is_letter('z'));
  STATIC_CHECK(not is_letter('0'));
}

TEST_CASE("Hexadecimal helpers encode and decode bytes", "[char_utils]")
{
  STATIC_CHECK(is_hex('0'));
  STATIC_CHECK(is_hex('a'));
  STATIC_CHECK(is_hex('F'));
  STATIC_CHECK(not is_hex('g'));

  STATIC_CHECK(to_digit('7') == 7);
  STATIC_CHECK(hex_to_char('0') == 0);
  STATIC_CHECK(hex_to_char('a') == 10);
  STATIC_CHECK(hex_to_char('F') == 15);
  STATIC_CHECK(hex_to_char('x') == static_cast<unsigned char>(-1));
  STATIC_CHECK(encode_hex(std::uint8_t {0}) == '0');
  STATIC_CHECK(encode_hex(std::uint8_t {10}) == 'a');
  STATIC_CHECK(encode_hex(std::uint8_t {15}) == 'f');
  STATIC_CHECK(hex_byte_join("00ff", 0) == 0x00);
  STATIC_CHECK(hex_byte_join("00ff", 2) == 0xff);
}
