//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_SET_CONCEPTS_HPP
#define XSTD_TEST_SET_CONCEPTS_HPP

#include <xstd/test/value_reference.hpp> // value_reference
#include <concepts>                      // regular, totally_ordered
#include <iterator>                      // bidirectional_iterator
#include <ranges>                        // bidirectional_range

namespace xstd::test::set {

// The interface a bit-packed set shares with the set it packs. The sequence
// counterpart is random-access over bool; this one is bidirectional over the
// keys, which is the whole difference between the two readings of a bitset.
template<class C>
concept bit_set =
        std::regular<C>
    and std::totally_ordered<C>
    and std::ranges::bidirectional_range<C>
    and std::bidirectional_iterator<typename C::iterator>
    and value_reference<typename C::const_reference>;

} // namespace xstd::test::set

#endif // XSTD_TEST_SET_CONCEPTS_HPP
