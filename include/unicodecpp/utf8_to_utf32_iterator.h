#pragma once

#include "unicodecpp/details.h"

#include <cassert>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace unicodecpp {

///
/// An iterator which transforms an UTF-8 byte sequence to UTF-32 character
/// sequence. It assumes that the underlying sequence is valid (causes undefined
/// behavior if not)
///
template <typename BaseIterator>
class Utf8ToUtf32Iterator
{
public:
  static_assert(
    details::HasCategory<BaseIterator, std::forward_iterator_tag>::value,
    "Not a ForwardIterator");

  using difference_type = std::ptrdiff_t;
  using value_type = uint32_t;
  using pointer = value_type *;
  using reference = value_type &;
  using iterator_category = typename std::conditional<
    details::HasCategory<BaseIterator, std::bidirectional_iterator_tag>::value,
    std::bidirectional_iterator_tag, std::forward_iterator_tag>::type;

  Utf8ToUtf32Iterator() {}

  Utf8ToUtf32Iterator(BaseIterator baseIter)
    : baseIter_(baseIter)
  {
  }

  const BaseIterator &baseIterator() const { return baseIter_; }

  bool operator==(const Utf8ToUtf32Iterator &rhs) const
  {
    return baseIter_ == rhs.baseIter_;
  }

  bool operator!=(const Utf8ToUtf32Iterator &rhs) const
  {
    return baseIter_ != rhs.baseIter_;
  }

  value_type operator*() const { return dereference(); }

  Utf8ToUtf32Iterator &operator++()
  {
    increment();
    return *this;
  }

  Utf8ToUtf32Iterator operator++(int)
  {
    auto res = *this;
    ++(*this);
    return res;
  }

  Utf8ToUtf32Iterator &operator--()
  {
    decrement();
    return *this;
  }

  Utf8ToUtf32Iterator operator--(int)
  {
    auto res = *this;
    --(*this);
    return res;
  }

private:
  uint32_t parseValue(BaseIterator baseIter) const
  {
    static constexpr uint32_t dataMask[5] = { 0 /* dummy value */, 0b11111111,
                                              0b00011111, 0b00001111,
                                              0b00000111 };

    uint32_t result = static_cast<uint8_t>(*baseIter);
    ++baseIter;

    int length = ::unicodecpp::details::parseUtf8CharLength(result);
    assert(1 <= length && length < (int)(sizeof(dataMask) / sizeof(*dataMask)));

    result &= dataMask[length];
    for (int i = 1; i < length; ++i) {
      uint32_t byte = static_cast<uint8_t>(*baseIter);
      ++baseIter;

      result <<= 6;
      result |= byte & 0b00111111;
    }

    return result;
  }

  void increment()
  {
    int length = ::unicodecpp::details::parseUtf8CharLength(*baseIter_);
    for (int i = 0; i < length; ++i)
      ++baseIter_;
  }

  void decrement()
  {
    uint8_t byte;
    do {
      byte = *(--baseIter_);
    } while (::unicodecpp::details::isUtf8ContinuationByte(byte));
  }

  bool equal(const Utf8ToUtf32Iterator &rhs) const
  {
    return baseIter_ == rhs.baseIter_;
  }

  uint32_t dereference() const { return parseValue(baseIter_); }

  BaseIterator baseIter_;
};

} // namespace unicodecpp

