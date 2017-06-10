#include "unicodecpp/v8_0_0/segmentation.h"

#include <algorithm>
#include <cassert>

namespace unicodecpp {
namespace v8_0_0 {
namespace {

#include "gb_tables.h"

// Returns a grapheme break property of the given code point.
GBProperty
gbProperty(uint32_t cp)
{
  if (cp > lastCodepoint)
    return GBProperty::None;
  auto sndLvlTable = gbPropertyTab[cp / sndLvlTableSize];
  return sndLvlTable ? sndLvlTable[cp % sndLvlTableSize] : GBProperty::None;
}

} // unnamed namespace

#define GRAPHEME_BREAK_USE_LOOKUP_TABLE
#ifndef GRAPHEME_BREAK_USE_LOOKUP_TABLE

bool
atGraphemeBreak(uint32_t leftCp, uint32_t rightCp)
{
  GBProperty lp = gbProperty(leftCp);
  GBProperty rp = gbProperty(rightCp);

  // GB3
  if (lp == GBProperty::CR && rp == GBProperty::LF)
    return false;

  // GB4
  if (lp == GBProperty::Control || lp == GBProperty::CR || lp == GBProperty::LF)
    return true;

  // GB5
  if (rp == GBProperty::Control || rp == GBProperty::CR || rp == GBProperty::LF)
    return true;

  // GB6
  if (lp == GBProperty::L && (rp == GBProperty::L || rp == GBProperty::V ||
                              rp == GBProperty::LV || rp == GBProperty::LVT))
    return false;

  // GB7
  if ((lp == GBProperty::LV || lp == GBProperty::V) &&
      (rp == GBProperty::V || rp == GBProperty::T))
    return false;

  // GB8
  if ((lp == GBProperty::LVT || lp == GBProperty::T) && rp == GBProperty::T)
    return false;

  // GB8a
  if (lp == GBProperty::Regional_Indicator &&
      rp == GBProperty::Regional_Indicator)
    return false;

  // GB9
  if (rp == GBProperty::Extend)
    return false;

  // GB9a
  if (rp == GBProperty::SpacingMark)
    return false;

  // GB9b: Currently there are no characters with this property
  // if (lp == GBProperty::Prepend)
  //  return false;

  // GB 10
  return true;
}

#else

// This is a more efficient implementation of the function above. Both are kept
// here for comparison.
bool
atGraphemeBreak(uint32_t leftCp, uint32_t rightCp)
{
  constexpr uint32_t N = static_cast<uint32_t>(GBProperty::Last);
  static constexpr bool lookup[N][N] = {
    { 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1 },
    { 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1 },
    { 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0 },
    { 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1 },
    { 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1 },
    { 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1 },
    { 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1 },
    { 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0 }
  };

  auto row = static_cast<uint32_t>(gbProperty(leftCp));
  auto col = static_cast<uint32_t>(gbProperty(rightCp));

  assert(row < N);
  assert(col < N);
  return lookup[row][col];
}

#endif // GRAPHEME_BREAK_USE_LOOKUP_TABLE

} // namespace v8_0_0
} // namespace unicodecpp

