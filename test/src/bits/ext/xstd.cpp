//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/ext/xstd.hpp>   // our own bitset made a range, asked for by name
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <ranges>                   // bidirectional_range

BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Xstd)

// The odd one of the three: it adapts a type this library already owns. It is
// separate from xstd/bits/bitset.hpp for the reason that header states -- a
// bitset is not a range, because std::bitset is not, and making ours one would
// be a difference from the type it reproduces. Iterating it is opt-in, and this
// header is the opting in.
BOOST_AUTO_TEST_CASE(TheOptInIsWhatMakesOurBitsetARange)
{
        static_assert(std::ranges::bidirectional_range<xstd::bitset<8>>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
