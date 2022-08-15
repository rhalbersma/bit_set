#pragma once

//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ranges>       // range
#include <utility>      // declval

namespace xstd {

template<class BitSet>
concept insertable = std::ranges::range<BitSet> && requires(BitSet&& bs)
{
        bs.insert(std::declval<typename BitSet::iterator>(), std::declval<typename BitSet::value_type>());
};

template<class BitSet>
concept resizeable = requires(BitSet&& bs)
{
        bs.resize(std::declval<typename BitSet::size_type>());
        bs.resize(std::declval<typename BitSet::size_type>(), std::declval<bool>());
};

}       // namespace xstd
