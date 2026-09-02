//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/ranges/block_access.hpp>      // block_access, block_range
#include <xstd/bits/bit_array.hpp>                // bit_array
#include <xstd/bits/bit_finite_set.hpp>           // bit_finite_set
#include <xstd/bits/bitset.hpp>                   // bitset
#include <xstd/bits/ranges/array_view.hpp>        // array_view
#include <xstd/bits/ranges/set_view.hpp>          // set_view
#include <xstd/bits/ext/std/bitset.hpp>           // block_access over std::bitset, where the words are reachable
#include <xstd/bits/ext/boost/dynamic_bitset.hpp> // the one that stays element-wise
#include <boost/test/unit_test.hpp>               // BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <algorithm>                              // lexicographical_compare
#include <bitset>                                 // bitset
#include <compare>                                // strong_ordering
#include <cstddef>                                // size_t
#include <cstdint>                                // uint8_t
#include <set>                                    // set
#include <vector>                                 // vector

BOOST_AUTO_TEST_SUITE(Bits)
BOOST_AUTO_TEST_SUITE(Ranges)
BOOST_AUTO_TEST_SUITE(BlockAccess)

namespace {

// Every ordered pair of subsets of an N-bit universe, with both orderings taken
// from the containers under test and from the standard containers holding the
// same content. The universe is small because the sweep is quadratic in 2^N; it
// is chosen per case to put the deciding bit in a different place each time.
template<std::size_t N, class Block>
auto sweep() -> void
{
        auto set_disagreements   = 0;
        auto array_disagreements = 0;

        for (auto i = 0UZ; i < (1UZ << N); ++i) {
                for (auto j = 0UZ; j < (1UZ << N); ++j) {
                        auto sx = xstd::bit_finite_set<N, Block>();
                        auto sy = xstd::bit_finite_set<N, Block>();
                        auto ax = xstd::bit_array<N, Block>();
                        auto ay = xstd::bit_array<N, Block>();
                        auto kx = std::set<std::size_t>();
                        auto ky = std::set<std::size_t>();
                        auto vx = std::vector<bool>(N);
                        auto vy = std::vector<bool>(N);

                        for (auto k = 0UZ; k < N; ++k) {
                                if (i >> k & 1UZ) { sx.insert(k); ax[k] = true; kx.insert(k); vx[k] = true; }
                                if (j >> k & 1UZ) { sy.insert(k); ay[k] = true; ky.insert(k); vy[k] = true; }
                        }

                        set_disagreements   += ((sx <=> sy) < 0) != std::lexicographical_compare(kx.begin(), kx.end(), ky.begin(), ky.end());
                        array_disagreements += ((ax <=> ay) < 0) != std::lexicographical_compare(vx.begin(), vx.end(), vy.begin(), vy.end());
                }
        }

        // Checked once rather than per pair: a million passing assertions say no
        // more than one, and drown the log if any of them fails.
        BOOST_CHECK_EQUAL(set_disagreements, 0);
        BOOST_CHECK_EQUAL(array_disagreements, 0);
}

// Dependent, so a standard library without the member is a substitution failure
// rather than a hard error -- asking c._Getword(0) of a concrete type that lacks
// it does not compile at all, requires-expression or no. Same reason
// ranges.cpp's has_address_of is written this way.
template<class B>
constexpr bool has_getword = requires (B const& c) { c._Getword(0UZ); };

} // namespace

// Which types answer where their blocks are, and therefore which take the
// word-at-a-time path. The views are here because a view is as block-accessible
// as the thing it views, which is what lets an ordering over set_view(bitset)
// run at the same speed as one over the bitset.
BOOST_AUTO_TEST_CASE(OursSayWhereTheirBlocksAre)
{
        using Block = std::uint8_t;

        static_assert(xstd::ranges::block_range<xstd::bit_finite_set<9, Block>>);
        static_assert(xstd::ranges::block_range<xstd::bit_array<9, Block>>);
        static_assert(xstd::ranges::block_range<xstd::bitset<9, Block>>);

        static_assert(xstd::ranges::block_range<xstd::set_view<xstd::bitset<9, Block>>>);
        static_assert(xstd::ranges::block_range<xstd::array_view<xstd::bitset<9, Block>>>);
}

// boost::dynamic_bitset has blocks and will not hand them over: num_blocks() is
// public but the buffer is not, and to_block_range copies, which is no use
// inside a noexcept comparison. It keeps the element-wise path, which is why
// that path stays rather than being replaced -- and it must still be right,
// which ranges/set_view.cpp checks against std::set.
BOOST_AUTO_TEST_CASE(ATypeThatWillNotHandThemOverKeepsTheElementWisePath)
{
        static_assert(not xstd::ranges::block_range<boost::dynamic_bitset<>>);
        static_assert(not xstd::ranges::block_range<xstd::set_view<boost::dynamic_bitset<>>>);
}

// std::bitset only where the standard library lets it: the Microsoft STL has a
// public _Getword, libstdc++ keeps its words private in _Base_bitset and libc++
// exposes nothing. Asserted as the disjunction rather than per platform, because
// which side holds is the standard library's business and not this library's --
// what matters is that the guard resolves one way or the other rather than
// failing to compile.
BOOST_AUTO_TEST_CASE(StdBitsetOnlyWhereItsWordsAreReachable)
{
        constexpr auto reachable = xstd::ranges::block_range<std::bitset<64>>;
        static_assert(reachable == has_getword<std::bitset<64>>);
}

// The word-at-a-time path against the containers that define the two orderings,
// at three shapes: inside one block, spanning two with the second nearly empty,
// and spanning two with padding above the last position. The block loop only
// does anything in the last two.
BOOST_AUTO_TEST_CASE(OneBlockExactly)
{
        sweep<8, std::uint8_t>();
}

BOOST_AUTO_TEST_CASE(TwoBlocksWithOneBitInTheSecond)
{
        sweep<9, std::uint8_t>();
}

BOOST_AUTO_TEST_CASE(TwoBlocksWithPaddingAboveTheLastPosition)
{
        sweep<10, std::uint8_t>();
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
