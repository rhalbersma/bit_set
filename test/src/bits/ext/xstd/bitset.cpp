//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/ext/xstd/bitset.hpp> // begin, end, rbegin, rend over xstd::bitset
#include <boost/test/unit_test.hpp>      // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <cstddef>                       // size_t
#include <iterator>                      // distance
#include <ranges>                        // begin, end, rbegin, rend, bidirectional_range
#include <vector>                        // vector

BOOST_AUTO_TEST_SUITE(Bits)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Xstd)
BOOST_AUTO_TEST_SUITE(Bitset)

// The ADL free functions this header adds route to the set reading, so iterating
// an xstd::bitset visits the positions that are set, in increasing order -- the
// keys, not the bools. That is the choice the header makes on the reader's
// behalf, and the one worth pinning.
BOOST_AUTO_TEST_CASE(IteratingVisitsTheOneBitsAsKeys)
{
        auto c = xstd::bitset<16>();
        c.set(1);
        c.set(7);
        c.set(15);

        static_assert(std::ranges::bidirectional_range<decltype(c)>);
        BOOST_CHECK_EQUAL(std::distance(std::ranges::begin(c), std::ranges::end(c)), 3);

        auto keys = std::vector<std::size_t>();
        for (auto k : c) {
                keys.push_back(k);
        }
        BOOST_CHECK((keys == std::vector<std::size_t>{1, 7, 15}));
}

// And backwards, which is the half a repeated find(first, last, true) cannot do.
BOOST_AUTO_TEST_CASE(AndBackwards)
{
        auto c = xstd::bitset<16>();
        c.set(1);
        c.set(7);
        c.set(15);

        auto keys = std::vector<std::size_t>();
        for (auto it = std::ranges::rbegin(c); it != std::ranges::rend(c); ++it) {
                keys.push_back(*it);
        }
        BOOST_CHECK((keys == std::vector<std::size_t>{15, 7, 1}));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
