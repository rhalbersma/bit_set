//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/ext/boost/dynamic_bitset.hpp> // bit_extent, set_find, set_compare, array_find
#include <xstd/test/dynamic.hpp>                  // dynamic
#include <boost/test/unit_test.hpp>               // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                               // regular, totally_ordered
#include <ranges>                                 // range
#include <type_traits>                            // is_trivially_copyable_v

BOOST_AUTO_TEST_SUITE(Bits)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(DynamicBitset)

using T = boost::dynamic_bitset<>;

BOOST_AUTO_TEST_CASE(Regular)
{
        static_assert(std::regular<T>);
}

// The one bitset here that does order itself: boost::dynamic_bitset has an infix
// <=>, where std::bitset and xstd::bitset both reach ordering only through the
// set reading. The view agrees with it rather than replacing it.
BOOST_AUTO_TEST_CASE(OrderedBothInfixAndThroughTheView)
{
        static_assert(std::totally_ordered<T>);
        static_assert(std::totally_ordered<xstd::set_view<T>>);
}

BOOST_AUTO_TEST_CASE(BothReadingsAreReachable)
{
        static_assert(xstd::ranges::set_range<T>);
        static_assert(xstd::ranges::array_range<T>);
}

// It owns its storage, which is what separates it from every other type here and
// what the sieve wants it for: the extent is a run-time value, so the trivial and
// nothrow batteries the fixed-width bitsets pass do not apply to it at all.
BOOST_AUTO_TEST_CASE(ItIsTheDynamicOne)
{
        static_assert(    xstd::test::dynamic<T>);
        static_assert(not std::is_trivially_copyable_v<T>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
