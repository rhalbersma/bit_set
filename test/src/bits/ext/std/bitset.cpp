//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <xstd/bits/bitset.hpp>         // bitset
#include <xstd/bits/ext/std/bitset.hpp> // bit_extent, set_find, set_compare, array_find
#include <bitset>                       // bitset
#include <concepts>                     // regular, totally_ordered
#include <ranges>                       // range
#include <tuple>                        // tuple
#include <type_traits>                  // is_nothrow_*, is_trivially_*

BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Std)
BOOST_AUTO_TEST_SUITE(Bitset)

// std::bitset takes no Block, so the graded extents do not apply: these are the widths a packed implementation would divide differently.
using Types = std::tuple
<       std::bitset<  0>
,       std::bitset<  1>
,       std::bitset< 64>
,       std::bitset<128>
,       std::bitset<192>
,       std::bitset<256>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IsRegular, T, Types)
{
        static_assert(std::regular<T>);
}

// The adaptor's whole job: std::bitset has no ordering, and none can legally be added to namespace std, so the set reading supplies it.
BOOST_AUTO_TEST_CASE_TEMPLATE(OrderedThroughTheViewRatherThanInfix, T, Types)
{
        static_assert(not std::totally_ordered<T>);
        static_assert(    std::totally_ordered<xstd::set_view<T>>);
}

// Both readings are reachable, which is what bit_extent, set_find and array_find are specialized here for.
BOOST_AUTO_TEST_CASE_TEMPLATE(BothReadingsAreReachable, T, Types)
{
        static_assert(not std::ranges::range<T>);
        static_assert(xstd::ranges::set_range<T>);
        static_assert(xstd::ranges::array_range<T>);
}

// Ours answers the same type-trait battery at the same widths, so nothing is given up by building it over the packed array.
BOOST_AUTO_TEST_CASE(OursReproducesItsTraits)
{
        using theirs = std::bitset<64>;
        using ours   = xstd::bitset<64>;

        static_assert(std::regular<theirs>       == std::regular<ours>);
        static_assert(std::totally_ordered<theirs> == std::totally_ordered<ours>);

        static_assert(std::is_nothrow_default_constructible_v<theirs> == std::is_nothrow_default_constructible_v<ours>);
        static_assert(std::is_nothrow_copy_constructible_v<theirs>    == std::is_nothrow_copy_constructible_v<ours>);
        static_assert(std::is_nothrow_move_assignable_v<theirs>       == std::is_nothrow_move_assignable_v<ours>);

        static_assert(std::is_trivially_destructible_v<theirs>       == std::is_trivially_destructible_v<ours>);
        static_assert(std::is_trivially_copy_constructible_v<theirs> == std::is_trivially_copy_constructible_v<ours>);
        static_assert(std::is_trivially_copy_assignable_v<theirs>    == std::is_trivially_copy_assignable_v<ours>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
