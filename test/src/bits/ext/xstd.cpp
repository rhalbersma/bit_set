//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <xstd/bits/ext/xstd.hpp>   // our own bitset made a range, asked for by name
#include <ranges>                   // bidirectional_range

BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Xstd)

// The odd one: it adapts a type this library already owns, because a bitset is not a range and iterating it is opt-in.
BOOST_AUTO_TEST_CASE(TheOptInIsWhatMakesOurBitsetARange)
{
        static_assert(std::ranges::bidirectional_range<xstd::bitset<8>>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
