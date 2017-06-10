#pragma once

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

} // namespace details
} // namespace unicodecpp

