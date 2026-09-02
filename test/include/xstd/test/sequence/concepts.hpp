//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_SEQUENCE_CONCEPTS_HPP
#define XSTD_TEST_SEQUENCE_CONCEPTS_HPP

#include <xstd/test/value_reference.hpp> // value_reference
#include <concepts>                      // regular, totally_ordered
#include <iterator>                      // random_access_iterator
#include <ranges>                        // random_access_range

namespace xstd::test::sequence {

// The interface a bit-packed sequence shares with the sequence it packs. Named
// here rather than restated per container, so the umbrella can say the two
// satisfy the same one instead of comparing two lists and hoping they stay equal.
template<class C>
concept bit_sequence =
        std::regular<C>
    and std::totally_ordered<C>
    and std::ranges::random_access_range<C>
    and std::random_access_iterator<typename C::iterator>
    and value_reference<typename C::const_reference>;

} // namespace xstd::test::sequence

#endif // XSTD_TEST_SEQUENCE_CONCEPTS_HPP
