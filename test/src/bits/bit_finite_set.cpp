//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/bit_finite_set.hpp> // bit_finite_set
#include <test/block_types.hpp>         // graded_extents
#include <test/set/concepts.hpp>        // bit_set
#include <test/value_reference.hpp>     // value_reference
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                     // regular, totally_ordered
#include <iterator>                     // bidirectional_iterator
#include <ranges>                       // bidirectional_range

BOOST_AUTO_TEST_SUITE(BitFiniteSet)

// Every Block model within one block, and the narrow ones across block
// boundaries too; the grading is in test/block_types.hpp.
using Types = test::graded_extents<xstd::bit_finite_set>;

// The clauses one at a time, so a failure names which one. The umbrella asserts
// the composite over this and over the std::set it packs.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsRegular, T, Types)
{
        static_assert(std::regular<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsTotallyOrdered, T, Types)
{
        static_assert(std::totally_ordered<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsABidirectionalRange, T, Types)
{
        static_assert(std::ranges::bidirectional_range<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ItsIteratorIsBidirectional, T, Types)
{
        using I = T::iterator;
        static_assert(std::bidirectional_iterator<I>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ItsConstReferenceIsAValue, T, Types)
{
        static_assert(test::value_reference<typename T::const_reference>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsABitSet, T, Types)
{
        static_assert(test::set::bit_set<T>);
}

BOOST_AUTO_TEST_SUITE_END()
