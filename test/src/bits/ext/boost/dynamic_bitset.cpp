//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>               // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <test/dynamic.hpp>                       // dynamic
#include <xstd/bits/ext/boost/dynamic_bitset.hpp> // bit_extent, set_find, set_compare, array_find
#include <algorithm>                              // lexicographical_compare
#include <compare>                                // strong_ordering
#include <concepts>                               // regular, totally_ordered
#include <cstddef>                                // size_t
#include <ranges>                                 // range
#include <set>                                    // set
#include <type_traits>                            // is_trivially_copyable_v

BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(DynamicBitset)

using T = boost::dynamic_bitset<>;

BOOST_AUTO_TEST_CASE(IsRegular)
{
        static_assert(std::regular<T>);
}

// The one bitset that orders itself, and wrongly: over a 4-bit universe its own < disagrees with std::set on 88 of 256 pairs, the view's on none.
BOOST_AUTO_TEST_CASE(TheViewReplacesItsOrderingRatherThanTrustingIt)
{
        static_assert(std::totally_ordered<T>);
        static_assert(std::totally_ordered<xstd::set_view<T>>);

        constexpr auto N = 4UZ;
        auto disagreements = 0;
        for (auto i = 0UZ; i < (1UZ << N); ++i) {
                for (auto j = 0UZ; j < (1UZ << N); ++j) {
                        auto x = T(N), y = T(N);
                        auto kx = std::set<std::size_t>(), ky = std::set<std::size_t>();
                        for (auto k = 0UZ; k < N; ++k) {
                                if (i >> k & 1UZ) { x.set(k); kx.insert(k); }
                                if (j >> k & 1UZ) { y.set(k); ky.insert(k); }
                        }

                        // What the keys themselves say, which is what the set reading means.
                        auto const expected = std::lexicographical_compare(kx.begin(), kx.end(), ky.begin(), ky.end());

                        BOOST_CHECK_EQUAL((xstd::set_view(x) <=> xstd::set_view(y)) < 0, expected);
                        disagreements += (x < y) != expected;
                }
        }

        // Not merely different in principle: different on a third of all pairs.
        BOOST_CHECK_EQUAL(disagreements, 88);
}

BOOST_AUTO_TEST_CASE(BothReadingsAreReachable)
{
        static_assert(xstd::ranges::set_range<T>);
        static_assert(xstd::ranges::array_range<T>);
}

// It owns its storage, so the extent is a run-time value and the trivial and nothrow batteries do not apply.
BOOST_AUTO_TEST_CASE(ItIsTheDynamicOne)
{
        static_assert(    test::dynamic<T>);
        static_assert(not std::is_trivially_copyable_v<T>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
