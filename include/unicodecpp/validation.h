#pragma once

#include "unicodecpp/details.h"

#include <cassert>

namespace unicodecpp {

///
/// Returns whether the given text is valid UTF-8. This function is specially
/// optimized for texts containing mostly ASCII characters. Optimization is
/// available only if InputIter = uint8_t*.
///
template <typename InputIter>
inline bool isValidUtf8(InputIter begin, InputIter end);

// Implementation

template <typename InputIter>
inline bool
isValidUtf8(InputIter begin, InputIter end)
{
  static constexpr uint32_t dataMask[] = { 0 /* dummy value */, 0b11111111,
                                           0b00011111, 0b00001111, 0b00000111 };

  while (begin != end) {
    ::unicodecpp::details::skipASCIIChars(begin, end);
    if (begin == end)
      break;

    // Parsing a non-ASCII character

    uint32_t codePoint = static_cast<uint8_t>(*begin);
    ++begin;
    int length = ::unicodecpp::details::parseUtf8CharLength(codePoint);
    if (length == -1)
      return false;

    assert(1 <= length && length < (int)(sizeof(dataMask) / sizeof(*dataMask)));
    codePoint &= dataMask[length];

    for (int i = 1; i < length; ++i, ++begin) {
      if (begin == end)
        return false;

      uint8_t byte = *begin;
      if (!::unicodecpp::details::isUtf8ContinuationByte(byte))
        return false;

      codePoint <<= 6;
      codePoint |= byte & 0b00111111;
    }

    // Verify that length is the shortest one which can encode the given code
    // point.
    if (!::unicodecpp::details::isValidUtf8CodePointLength(length, codePoint))
      return false;
  }

  return true;
}

} // namespace unicodecpp

