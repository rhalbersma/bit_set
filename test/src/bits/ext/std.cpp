//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <xstd/bits/ext/std.hpp>           // the std adaptors, asked for by name
#include <xstd/bits/ranges/array_view.hpp> // array_range
#include <xstd/bits/ranges/set_view.hpp>   // set_range
#include <bitset>                          // bitset
#include <span>                            // dynamic_extent

BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Std)

// One adapted library and no umbrella above it, so the front door never puts someone else's headers on a consumer's path.
BOOST_AUTO_TEST_CASE(AskingForItByNameIsEnough)
{
        static_assert(xstd::ranges::set_range<std::bitset<8>>);
        static_assert(xstd::ranges::array_range<std::bitset<8>>);
        static_assert(xstd::ranges::bit_extent<std::bitset<8>> == 8);
        static_assert(xstd::ranges::bit_extent<std::bitset<8>> != std::dynamic_extent);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
