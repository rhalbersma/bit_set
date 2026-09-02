//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/bitset.hpp>      // bitset
#include <xstd/test/block_types.hpp> // graded_extents
#include <boost/test/unit_test.hpp>  // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                  // regular, totally_ordered
#include <type_traits>               // is_nothrow_*, is_trivially_*

BOOST_AUTO_TEST_SUITE(Bits)
BOOST_AUTO_TEST_SUITE(Bitset)

using Types = xstd::test::graded_extents<xstd::bitset>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Regular, T, Types)
{
        static_assert(std::regular<T>);
}

// Deliberately not orderable by itself, because std::bitset is not: this type
// reproduces that one, and an infix <=> it does not have would be a difference,
// not a bonus. The ordering is reached the same way std::bitset's is, through
// the set reading -- see xstd/bits/ext/std/bitset.hpp for why that is the only
// place it can live for std::bitset, and why ours follows suit.
BOOST_AUTO_TEST_CASE_TEMPLATE(OrderedThroughTheViewRatherThanInfix, T, Types)
{
        static_assert(not std::totally_ordered<T>);
        static_assert(    std::totally_ordered<xstd::set_view<T>>);
}

// A fixed-width bitset owns no storage, so every operation on it is nothrow.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsNoThrow, T, Types)
{
        static_assert(std::is_nothrow_destructible_v<T>);
        static_assert(std::is_nothrow_default_constructible_v<T>);
        static_assert(std::is_nothrow_copy_constructible_v<T>);
        static_assert(std::is_nothrow_copy_assignable_v<T>);
        static_assert(std::is_nothrow_move_constructible_v<T>);
        static_assert(std::is_nothrow_move_assignable_v<T>);
}

// Trivial in every respect but default construction, which zeroes the bits.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsTrivial, T, Types)
{
        static_assert(    std::is_trivially_destructible_v<T>);
        static_assert(not std::is_trivially_default_constructible_v<T>);
        static_assert(    std::is_trivially_copy_constructible_v<T>);
        static_assert(    std::is_trivially_copy_assignable_v<T>);
        static_assert(    std::is_trivially_move_constructible_v<T>);
        static_assert(    std::is_trivially_move_assignable_v<T>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
