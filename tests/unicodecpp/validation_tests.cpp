#include "unicodecpp/validation.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

using ::testing::Combine;
using ::testing::ValuesIn;

namespace {

struct IsValidUtf8Test
  : public ::testing::TestWithParam<std::tuple<bool, std::string>>
{
};

TEST_P(IsValidUtf8Test, ReturnsTrueIfValid)
{
  using ::unicodecpp::isValidUtf8;

  const auto &param = GetParam();
  bool valid = std::get<0>(param);
  auto data = std::get<1>(param);

  // Tests uint8_t* overload
  uint8_t *begin = reinterpret_cast<uint8_t *>(&data[0]);
  uint8_t *end = begin + data.size();
  EXPECT_EQ(valid, isValidUtf8(begin, end));

  // Tests const uint8_t* overload
  const uint8_t *cBegin = begin;
  const uint8_t *cEnd = end;
  EXPECT_EQ(valid, isValidUtf8(cBegin, cEnd));

  // Tests iterator overload
  EXPECT_EQ(valid, isValidUtf8(data.begin(), data.end()));
}

std::vector<std::string> validUtf8 = {
  "0123456789abcdef0123"
  "\x00"
  "\x7F"
  "\xC2\x80"
  "\xDF\xBF"
  "\xE0\xA0\x80"
  "\xEF\xBF\xBF"
  "\xF2\x80\x80\x80"
  "\xF4\x8C\xBF\xBF"
  "0123456789abcdef",
  "0123456789abcdefghijklmnoprstuvw"
};

INSTANTIATE_TEST_CASE_P(ValidUtf8, IsValidUtf8Test,
                        Combine(ValuesIn({ true }), ValuesIn(validUtf8)));

std::vector<std::string> invalidUtf8 = {
  // Premature eof
  "0123456789abcdef0123\xC0",
  "0123456789abcdef0123\xE0",
  "0123456789abcdef0123\xE0\xA0",
  "0123456789abcdef0123\xF0",
  "0123456789abcdef0123\xF0\xA0",
  "0123456789abcdef0123\xF0\xA0\xA0",

  // Not enough continuation bytes
  "0123456789abcdef0123\xC0""0123456789abcdef",
  "0123456789abcdef0123\xE0""0123456789abcdef",
  "0123456789abcdef0123\xE0\xA0""0123456789abcdef",
  "0123456789abcdef0123\xF0""0123456789abcdef",
  "0123456789abcdef0123\xF0\xA0""0123456789abcdef",
  "0123456789abcdef0123\xF0\xA0\xA0""0123456789abcdef",

  // Sequences which can be encoded using shorter sequences
  "0123456789abcdef0123\xC1\xBF""0123456789abcdef",
  "0123456789abcdef0123\xE0\x0F\xBF""0123456789abcdef",
  "0123456789abcdef0123\xF0\x83\xBF\xFF""0123456789abcdef",

  // Too big value
  "0123456789abcdef0123\xF4\x90\x80\x80""0123456789abcdef",

  // Invalid multibyte start
  "0123456789abcdef0123\xF8""0123456789abcdef",
  "0123456789abcdef0123\xFC""0123456789abcdef",
  "0123456789abcdef0123\xFE""0123456789abcdef",
  "0123456789abcdef0123\xFF""0123456789abcdef",
};

INSTANTIATE_TEST_CASE_P(InvalidUtf8, IsValidUtf8Test,
                        Combine(ValuesIn({ false }), ValuesIn(invalidUtf8)));

TEST(IsUtf8StartByteTest, ReturnsTrueForStartBytes)
{
  using ::unicodecpp::isUtf8StartByte;
  EXPECT_TRUE(isUtf8StartByte(0b01111111));
  EXPECT_TRUE(isUtf8StartByte(0b11011111));
  EXPECT_TRUE(isUtf8StartByte(0b11101111));
  EXPECT_TRUE(isUtf8StartByte(0b11110111));
}

TEST(IsUtf8StartByteTest, ReturnsFalseForNonStartBytes)
{
  using ::unicodecpp::isUtf8StartByte;
  EXPECT_FALSE(isUtf8StartByte(0b10000000));
  EXPECT_FALSE(isUtf8StartByte(0b11111011));
  EXPECT_FALSE(isUtf8StartByte(0b11111101));
  EXPECT_FALSE(isUtf8StartByte(0b11111110));
  EXPECT_FALSE(isUtf8StartByte(0b11111111));
}

} // unnamed namespace

