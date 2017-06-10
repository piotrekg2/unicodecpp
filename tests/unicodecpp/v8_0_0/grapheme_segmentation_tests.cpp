#include "unicodecpp/v8_0_0/segmentation.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cassert>
#include <vector>

using ::testing::ValuesIn;

using ::unicodecpp::v8_0_0::atGraphemeBreak;
using ::unicodecpp::v8_0_0::advanceToNextGraphemeBreak;
using ::unicodecpp::v8_0_0::advanceToPrevGraphemeBreak;

namespace {

using Testcase = std::pair<std::vector<uint32_t>, std::vector<bool>>;

struct GraphemeBreakTest : public ::testing::TestWithParam<Testcase>
{
};

TEST_P(GraphemeBreakTest, AtGraphemeBreak)
{
  auto &param = GetParam();
  const std::vector<uint32_t> &str = param.first;
  const std::vector<bool> &atBreak = param.second;

  for (auto i = 0u; i < atBreak.size(); ++i) {
    EXPECT_EQ(atBreak[i],
              atGraphemeBreak(str.begin(), str.begin() + i, str.end()));
  }
}

TEST_P(GraphemeBreakTest, AdvanceToPrevBreak)
{
  auto &param = GetParam();
  const std::vector<uint32_t> &str = param.first;
  const std::vector<bool> &atBreak = param.second;

  int breakCnt = std::count(atBreak.begin(), atBreak.end(), 1);

  auto current = str.end();
  int actualBreakCnt = 1; // count str.end();
  while (advanceToPrevGraphemeBreak(str.begin(), current)) {
    EXPECT_EQ(1, atBreak[current - str.begin()]);
    ++actualBreakCnt;
  }
  EXPECT_EQ(breakCnt, actualBreakCnt);
}

TEST_P(GraphemeBreakTest, AdvanceToNextBreak)
{
  auto &param = GetParam();
  const std::vector<uint32_t> &str = param.first;
  const std::vector<bool> &atBreak = param.second;

  int breakCnt = std::count(atBreak.begin(), atBreak.end(), 1);

  auto current = str.begin();
  int actualBreakCnt = 1; // count str.begin();
  while (advanceToNextGraphemeBreak(current, str.end())) {
    EXPECT_EQ(1, atBreak[current - str.begin()]);
    ++actualBreakCnt;
  }
  EXPECT_EQ(breakCnt, actualBreakCnt);
}

#include "gb_tests.h"

INSTANTIATE_TEST_CASE_P(GeneratedTest, GraphemeBreakTest,
                        ValuesIn(generatedTests));

} // unnamed namespace

