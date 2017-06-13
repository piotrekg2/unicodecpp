// This is an internal header, do not include directly.
#pragma once

#ifdef _MSC_VER
#include <intrin.h>
#endif // _MSC_VER

#ifdef __AVX2__
#include <immintrin.h>
#endif // __AVX2__

#ifdef __SSE2__
#include <emmintrin.h>
#endif // __SSE2__

#include <algorithm>
#include <cassert>
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

///
/// Returns whether the given value is the shortest length of the sequence
/// encoding the given code point. According to the Unicode specification
/// only the shortest possible sequence of bytes can correctly encode a code
/// point. Code points outside of valid range [0, 0x10FFFF] have no valid
/// lengths.
///
inline bool
isValidUtf8CodePointLength(int length, uint32_t codePoint)
{
  static const uint32_t maxValues[] = { /* sentinel */ uint32_t(-1), 0x7F,
                                        0x7FF, 0xFFFF, 0x10FFFF };
  return 1 <= length && length <= 4 && maxValues[length - 1] + 1 <= codePoint &&
         codePoint <= maxValues[length];
}

///
/// Returns the number of trailing zero bits in a number.
///
/// Causes undefined behavior for n = 0; example: countTrailingZeros(8) = 3.
///
inline int
countTrailingZeros(unsigned n)
{
  assert(n != 0);

#ifdef _MSC_VER
  unsigned long ul;
  _BitScanForward(&ul, n);
  return ul;
#else
  return __builtin_ctz(n);
#endif // _MSC_VER
}

template <typename InputIter>
void skipASCIIChars(InputIter &begin, InputIter end);

///
/// Advances a given pointer to the first non-ASCII byte.
///
template <typename CharT>
inline void
skipASCIICharsFast(CharT *&begin, CharT *end)
{
  using CharType = typename std::remove_cv<CharT>::type;
  static_assert(std::is_same<CharType, uint8_t>::value,
                "CharType must be uint8_t");

  assert(begin <= end);

// An optimization based on available instruction set
#ifdef __AVX2__
  while (end - begin >= 32) {
    __m256i chunk =
      _mm256_loadu_si256(reinterpret_cast<const __m256i *>(begin));
    unsigned mask = _mm256_movemask_epi8(chunk);
    if (!mask) {
      begin += 32;
      continue;
    }

    begin += countTrailingZeros(mask);
    return;
  }
#elif defined __SSE2__
  while (end - begin >= 16) {
    __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i *>(begin));
    unsigned mask = _mm_movemask_epi8(chunk);
    if (!mask) {
      begin += 16;
      continue;
    }

    begin += countTrailingZeros(mask);
    return;
  }
#endif // __AVX2__

  // General solution
  skipASCIIChars<CharT *>(begin, end);
}

// An overload for all iterators
template <typename InputIter>
inline void
skipASCIIChars(InputIter &begin, InputIter end)
{
  begin =
    std::find_if(begin, end, [](uint8_t c) { return (c & 0b10000000) != 0; });
}

inline void
skipASCIIChars(uint8_t *&begin, uint8_t *end)
{
  skipASCIICharsFast(begin, end);
}

inline void
skipASCIIChars(const uint8_t *&begin, const uint8_t *end)
{
  skipASCIICharsFast(begin, end);
}

} // namespace details
} // namespace unicodecpp

