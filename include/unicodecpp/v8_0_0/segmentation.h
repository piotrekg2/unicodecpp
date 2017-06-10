///
/// This is an implementation of unicode text segmentation. The specification
/// is available at http://www.unicode.org/reports/tr29/tr29-27.html
///
#pragma once

#include "unicodecpp/details.h"

#include <cstdint>
#include <iterator>

namespace unicodecpp {
namespace v8_0_0 {

///
/// Returns whether there is a grapheme break between the two adjacent
/// codepoints.
///
/// This function cannot detect grapheme breaks at the begin or end of
/// the text.
///
bool atGraphemeBreak(uint32_t leftCp, uint32_t rightCp);

///
/// Returns whether there is a grapheme break at the given position.
///
/// @param begin the iterator to the first code point of the text.
/// @param current the current position.
/// @param end the iterator to the one past last code point of the text.
///
template <typename Iter>
bool atGraphemeBreak(Iter begin, Iter current, Iter end);

///
/// Advances the given iterator to the first grapheme break located before the
/// current position. Returns true iff the operation succeeded (it fails only
/// iff current == begin).
///
/// @param begin the iterator to the first code point of the whole text.
/// @param current the iterator to modify.
///
template <typename Iter>
bool advanceToPrevGraphemeBreak(Iter begin, Iter &current);

///
/// Advances the given iterator to the first grapheme break located after the
/// current position. Returns true iff the operation succeeded (it fails only
/// iff current == end).
///
/// @param current the iterator to advance.
/// @param end the iterator to the one past last codepoint of the whole text.
///
template <typename Iter>
bool advanceToNextGraphemeBreak(Iter &current, Iter end);

// Implementation

template <typename Iter>
inline bool
atGraphemeBreak(Iter begin, Iter current, Iter end)
{
  if (current == begin || current == end)
    return true;
  uint32_t currentCp = *current;
  --current;
  return ::unicodecpp::v8_0_0::atGraphemeBreak(*current, currentCp);
}

template <typename Iter>
inline bool
advanceToPrevGraphemeBreak(Iter begin, Iter &current)
{
  static_assert(
    details::HasCategory<Iter, std::bidirectional_iterator_tag>::value,
    "Not a BidirectionalIterator");

  if (current == begin)
    return false;

  --current;
  while (current != begin) {
    auto prev = current;
    --prev;
    if (::unicodecpp::v8_0_0::atGraphemeBreak(*prev, *current))
      break;
    current = prev;
  }
  return true;
}

template <typename Iter>
inline bool
advanceToNextGraphemeBreak(Iter &current, Iter end)
{
  static_assert(details::HasCategory<Iter, std::forward_iterator_tag>::value,
                "Not a ForwardIterator");

  if (current == end)
    return false;

  while (true) {
    uint32_t prevCp = *current;
    ++current;
    if (current == end ||
        ::unicodecpp::v8_0_0::atGraphemeBreak(prevCp, *current))
      break;
  }
  return true;
}

} // namespace v8_0_0
} // namespace unicodecpp

