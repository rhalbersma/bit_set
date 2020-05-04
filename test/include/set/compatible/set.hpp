#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>    // lexicographical_compare_three_way
#include <compare>      // strong_ordering
#include <set>          // set

namespace xstd {

// provided by <set> in C++20
template<class Compare, class Allocator>
[[nodiscard]] auto operator<=>(std::set<int, Compare, Allocator> const& lhs, std::set<int, Compare, Allocator> const& rhs) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

}       // namespace xstd
