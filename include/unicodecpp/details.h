// This is an internal header, do not include directly.
#pragma once

#include <cstdint>
#include <iterator>
#include <type_traits>

namespace unicodecpp {
namespace details {

///
/// An utility class for checking iterator category.
///
template <typename IteratorT, typename CategoryTag>
struct HasCategory
{
  using ActualCategoryTag =
    typename std::iterator_traits<IteratorT>::iterator_category;

  static constexpr bool value =
    std::is_base_of<CategoryTag, ActualCategoryTag>::value;
};

///
/// Parses the length of a character from the first byte of a sequence encoding
/// it. Returns -1 if the given byte is not the first byte of the sequence.
///
inline int
parseUtf8CharLength(uint8_t byte)
{
  /* Equivalent code:
  if ((byte & 0b10000000) == 0b00000000)
    return 1;
  else if ((byte & 0b11100000) == 0b11000000)
    return 2;
  else if ((byte & 0b11110000) == 0b11100000)
    return 3;
  else if ((byte & 0b11111000) == 0b11110000)
    return 4;
  return -1;
  */
  static const int lookup[] = { 1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,
                                1,  1,  1, 1, 1, -1, -1, -1, -1, -1, -1,
                                -1, -1, 2, 2, 2, 2,  3,  3,  4,  -1 };
  return lookup[byte >> 3];
}

///
/// Returns whether a given byte is an UTF-8 continuation byte.
///
inline bool
isUtf8ContinuationByte(uint8_t byte)
{
  return (byte & 0b11000000) == 0b10000000;
}

} // namespace details
} // namespace unicodecpp

