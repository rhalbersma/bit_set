//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/bit_array.hpp>            // bit_array
#include <xstd/test/block_types.hpp>          // graded_extents
#include <xstd/test/sequence/concepts.hpp>    // bit_sequence, value_reference
#include <boost/test/unit_test.hpp>           // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                           // regular, totally_ordered
#include <iterator>                           // random_access_iterator
#include <ranges>                             // random_access_range

BOOST_AUTO_TEST_SUITE(Bits)
BOOST_AUTO_TEST_SUITE(BitArray)

// Every Block model within one block, and the narrow ones across block
// boundaries too. The grading is in xstd/test/block_types.hpp, shared with the
// other two containers taking <size_t N, class Block>.
using Types = xstd::test::graded_extents<xstd::bit_array>;

// The clauses one at a time, so a failure names which one. The umbrella asserts
// the composite over this and over the std::array it packs.
BOOST_AUTO_TEST_CASE_TEMPLATE(Regular, T, Types)
{
        static_assert(std::regular<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TotallyOrdered, T, Types)
{
        static_assert(std::totally_ordered<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(RandomAccessRange, T, Types)
{
        static_assert(std::ranges::random_access_range<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(RandomAccessIterator, T, Types)
{
        using I = T::iterator;
        static_assert(std::random_access_iterator<I>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ConstReference, T, Types)
{
        static_assert(xstd::test::sequence::value_reference<typename T::const_reference>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitSequence, T, Types)
{
        static_assert(xstd::test::sequence::bit_sequence<T>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
