#include "unicodecpp/utf8_to_utf32_iterator.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace {

using Testcase = std::pair<std::string, std::u32string>;

struct Utf8ToUtf32IteratorTest : public ::testing::TestWithParam<Testcase>
{
};

TEST_P(Utf8ToUtf32IteratorTest, ForwardIteration)
{
  auto &param = GetParam();
  const std::string &utf8Text = param.first;
  const std::u32string &utf32Text = param.second;

  using Iter = unicodecpp::Utf8ToUtf32Iterator<std::string::const_iterator>;
  auto begin = Iter(utf8Text.begin());
  auto end = Iter(utf8Text.end());

  EXPECT_EQ(std::distance(utf32Text.begin(), utf32Text.end()),
            std::distance(begin, end));
  EXPECT_TRUE(std::equal(begin, end, utf32Text.begin()));
}

TEST_P(Utf8ToUtf32IteratorTest, BackwardIteration)
{
  auto &param = GetParam();
  const std::string &utf8Text = param.first;
  const std::u32string &utf32Text = param.second;

  using Iter =
    unicodecpp::Utf8ToUtf32Iterator<std::string::const_reverse_iterator>;

  auto rbegin = Iter(utf8Text.rbegin());
  auto rend = Iter(utf8Text.rend());

  EXPECT_EQ(std::distance(utf32Text.rbegin(), utf32Text.rend()),
            std::distance(rbegin, rend));
  EXPECT_TRUE(std::equal(rbegin, rend, utf32Text.rbegin()));
}

const std::vector<Testcase> singleByteSequences = {
  { u8"", U"" },
  { u8"abcefghijklmnoprstuvwxyz", U"abcefghijklmnoprstuvwxyz" }
};

INSTANTIATE_TEST_CASE_P(SingleByteSequence, Utf8ToUtf32IteratorTest,
                        ::testing::ValuesIn(singleByteSequences));

const std::vector<Testcase> multiByteSequences = {
  { u8"abc\u0000\u0080\u0800\U00010000", U"abc\u0000\u0080\u0800\U00010000" }
};

INSTANTIATE_TEST_CASE_P(MultiByteSequence, Utf8ToUtf32IteratorTest,
                        ::testing::ValuesIn(multiByteSequences));

} // unnamed namespace

