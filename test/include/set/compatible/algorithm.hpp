#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>    // lexicographical_compare_three_way
#include <compare>      // strong_ordering

namespace xstd {

#if __cpp_lib_three_way_comparison >= 201907L

template< class InputIt1, class InputIt2>
constexpr auto lexicographical_compare_three_way(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
        return std::lexicographical_compare_three_way(first1, last1, first2, last2);
}

#else

template< class InputIt1, class InputIt2>
constexpr auto lexicographical_compare_three_way(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
        for (/* no initalization */; first1 != last1 && first2 != last2; void(++first1), void(++first2) ) {
                if (auto cmp = *first1 <=> *first2; cmp != 0) {
                        return cmp;
                }
        }
        return
                first1 != last1 ? std::strong_ordering::greater :
                first2 != last2 ? std::strong_ordering::less    :
                                  std::strong_ordering::equal
        ;
}

#endif

}       // namespace xstd
