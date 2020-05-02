#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/container/flat_set.hpp> // flat_set
#include <set/compatible/algorithm.hpp> // lexicographical_compare_three_way

namespace boost::container {

template<class Compare, class Allocator>
auto operator<=>(flat_set<int, Compare, Allocator> const& lhs, flat_set<int, Compare, Allocator> const& rhs) noexcept
        -> std::strong_ordering
{
        return xstd::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

}       // namespace boost::container
