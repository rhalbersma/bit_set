//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/ranges/set_view.hpp>          // set_view, set_range
#include <xstd/bits/bitset.hpp>                   // bitset
#include <xstd/bits/ext/std/bitset.hpp>           // set_find, set_compare over std::bitset
#include <xstd/bits/ext/boost/dynamic_bitset.hpp> // set_find, set_compare over boost::dynamic_bitset
#include <test/set/ordering.hpp>                  // ordering_agrees_with_std_set
#include <boost/test/unit_test.hpp>               // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <bitset>                                 // bitset
#include <concepts>                               // totally_ordered
#include <cstddef>                                // size_t
#include <ranges>                                 // bidirectional_range

BOOST_AUTO_TEST_SUITE(Ranges)
BOOST_AUTO_TEST_SUITE(SetView)

// The types a set_view exists for: the ones that hold a set of positions without
// offering it. bit_finite_set is not among them -- it is already a set, and
// wrapping it would be viewing a thing as itself.
BOOST_AUTO_TEST_CASE(TheViewedTypesAreTheOnesHoldingASetWithoutOfferingIt)
{
        static_assert(xstd::ranges::set_range<std::bitset<8>>);
        static_assert(xstd::ranges::set_range<xstd::bitset<8>>);
        static_assert(xstd::ranges::set_range<boost::dynamic_bitset<>>);

        // None of them is a range on its own; that is what the view supplies.
        static_assert(not std::ranges::range<std::bitset<8>>);
        static_assert(not std::ranges::range<xstd::bitset<8>>);

        static_assert(std::ranges::bidirectional_range<xstd::set_view<std::bitset<8>>>);
        static_assert(std::ranges::bidirectional_range<xstd::set_view<xstd::bitset<8>>>);
        static_assert(std::ranges::bidirectional_range<xstd::set_view<boost::dynamic_bitset<>>>);
}

// The ordering the view supplies is the set ordering, checked against std::set
// rather than against a restatement of it -- for every viewed type, including
// the one whose own <=> disagrees.
BOOST_AUTO_TEST_CASE(EveryViewedTypeOrdersLikeAStdSet)
{
        test::set::ordering_agrees_with_std_set<std::bitset<8>>();
        test::set::ordering_agrees_with_std_set<xstd::bitset<8>>();
        test::set::ordering_agrees_with_std_set<boost::dynamic_bitset<>>();
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
