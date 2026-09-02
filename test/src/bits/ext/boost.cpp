//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <xstd/bits/ext/boost.hpp>  // the Boost adaptors, asked for by name
#include <span>                     // dynamic_extent

BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)

// As for the std umbrella, asking by name is enough; this is the only adapted type carrying its width in the object.
BOOST_AUTO_TEST_CASE(AskingForItByNameIsEnough)
{
        static_assert(xstd::ranges::set_range<boost::dynamic_bitset<>>);
        static_assert(xstd::ranges::array_range<boost::dynamic_bitset<>>);
        static_assert(xstd::ranges::bit_extent<boost::dynamic_bitset<>> == std::dynamic_extent);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
