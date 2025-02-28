#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/container/flat_set.hpp> // flat_set
#include <algorithm>                    // lexicographical_compare_three_way

namespace boost::container {

template<class Key, class Compare, class KeyContainer>
[[nodiscard]] auto operator<=>(flat_set<Key, Compare, KeyContainer> const& x, flat_set<Key, Compare, KeyContainer> const& y) noexcept
{
        return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end());
}

}       // namespace boost::container
