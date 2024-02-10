#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/container/flat_set.hpp> // flat_set
#include <algorithm>                    // lexicographical_compare_three_way

namespace boost::container {

template<class Key, class Compare, class KeyContainer>
[[nodiscard]] auto operator<=>(flat_set<Key, Compare, KeyContainer> const& lhs, flat_set<Key, Compare, KeyContainer> const& rhs) noexcept
{
        return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

}       // namespace boost::container
