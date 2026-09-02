//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/ranges/array_view.hpp>        // array_view, array_range
#include <xstd/bits/bit_array.hpp>                // bit_array
#include <xstd/bits/bit_finite_set.hpp>           // bit_finite_set
#include <xstd/bits/bitset.hpp>                   // array_find over xstd::bitset
#include <xstd/bits/ext/std/bitset.hpp>           // array_find over std::bitset
#include <xstd/bits/ext/boost/dynamic_bitset.hpp> // array_find over boost::dynamic_bitset
#include <boost/test/unit_test.hpp>               // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <algorithm>                              // equal
#include <array>                                  // array
#include <bitset>                                 // bitset
#include <cstddef>                                // size_t
#include <ranges>                                 // random_access_range

BOOST_AUTO_TEST_SUITE(Bits)
BOOST_AUTO_TEST_SUITE(Ranges)
BOOST_AUTO_TEST_SUITE(ArrayView)

BOOST_AUTO_TEST_CASE(TheViewedTypesAreTheOnesHoldingBoolsWithoutOfferingThem)
{
        static_assert(xstd::ranges::array_range<std::bitset<8>>);
        static_assert(xstd::ranges::array_range<xstd::bitset<8>>);
        static_assert(xstd::ranges::array_range<boost::dynamic_bitset<>>);

        static_assert(std::ranges::random_access_range<xstd::array_view<std::bitset<8>>>);
        static_assert(std::ranges::random_access_range<xstd::array_view<xstd::bitset<8>>>);

        // bit_finite_set is not a sequence of bools; it is a set of keys.
        static_assert(not xstd::ranges::array_range<xstd::bit_finite_set<8>>);
}

// The sequence reading is the bools at every position, in position order,
// checked against the std::array<bool, N> holding the same bits.
BOOST_AUTO_TEST_CASE(TheSequenceReadingIsTheArrayOfBools)
{
        constexpr auto N = 8UZ;
        for (auto i = 0UZ; i < (1UZ << N); ++i) {
                auto packed = xstd::bitset<N>();
                auto plain  = std::array<bool, N>{};
                for (auto k = 0UZ; k < N; ++k) {
                        if (i >> k & 1UZ) { packed.set(k); plain[k] = true; }
                }

                auto const view = xstd::array_view(packed);
                BOOST_CHECK_EQUAL(view.size(), N);
                BOOST_CHECK(std::ranges::equal(view, plain));
        }
}

// A view is mutable through: writing a position through the view writes the bit,
// which is what separates it from the read-only adaptor it replaced.
BOOST_AUTO_TEST_CASE(WritingThroughTheViewWritesTheBits)
{
        auto packed = xstd::bitset<8>();
        auto view = xstd::array_view(packed);

        view[3] = true;
        BOOST_CHECK(packed.test(3));
        BOOST_CHECK_EQUAL(packed.count(), 1);

        view[3] = false;
        BOOST_CHECK(not packed.test(3));
        BOOST_CHECK(packed.none());
}

// The same reading over the type this library packs, so the view is not merely
// self-consistent: bit_array's own operator[] and the view agree position by
// position.
BOOST_AUTO_TEST_CASE(APackedArrayAgreesWithItsOwnView)
{
        auto packed = xstd::bit_array<8, unsigned char>{};
        packed[1] = true;
        packed[6] = true;

        auto const view = xstd::array_view(packed);
        for (auto k = 0UZ; k < 8UZ; ++k) {
                BOOST_CHECK_EQUAL(static_cast<bool>(view[k]), static_cast<bool>(packed[k]));
        }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
