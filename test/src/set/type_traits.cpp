//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <set/flat_set.hpp>             // XSTD_TEST_HAS_FLAT_SET
#include <xstd/bits/set/finite_bit_set.hpp>             // finite_bit_set
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <concepts>                     // equality_comparable, regular, totally_ordered
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <iterator>                     // bidirectional_iterator
#include <ranges>                       // bidirectional_range
#include <set>                          // set
#include <tuple>                        // tuple
#include <type_traits>                  // is_default_xxx_v, is_reference_v, is_trivially_xxx_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

using namespace xstd;

using Types = std::tuple
<       std::set<std::size_t>
#ifdef XSTD_TEST_HAS_FLAT_SET
,       std::flat_set<std::size_t>
#endif
,       finite_bit_set<  0, uint8_t>
,       finite_bit_set<  8, uint8_t>
,       finite_bit_set< 16, uint8_t>
,       finite_bit_set< 24, uint8_t>
,       finite_bit_set<  0, uint16_t>
,       finite_bit_set< 16, uint16_t>
,       finite_bit_set< 32, uint16_t>
,       finite_bit_set< 48, uint16_t>
,       finite_bit_set<  0, uint32_t>
,       finite_bit_set< 32, uint32_t>
,       finite_bit_set< 64, uint32_t>
,       finite_bit_set< 96, uint32_t>
,       finite_bit_set<  0, uint64_t>
,       finite_bit_set< 64, uint64_t>
,       finite_bit_set<128, uint64_t>
,       finite_bit_set<192, uint64_t>
#if defined(__GNUG__)
,       finite_bit_set<  0, __uint128_t>
,       finite_bit_set<128, __uint128_t>
,       finite_bit_set<256, __uint128_t>
,       finite_bit_set<384, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Regular, T, Types)
{
        static_assert(std::regular<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TotallyOrdered, T, Types)
{
        static_assert(std::totally_ordered<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BidirectionalRange, T, Types)
{
        static_assert(std::ranges::bidirectional_range<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BidirectionalIterator, T, Types)
{
        using I = T::iterator;
        static_assert(std::bidirectional_iterator<I>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ConstReference, T, Types)
{
        using R = T::const_reference;
        static_assert(    std::is_trivially_destructible_v<R>);
        static_assert(not std::is_default_constructible_v<R>);
        static_assert(    std::is_trivially_copy_constructible_v<R>);
        static_assert(not std::is_copy_assignable_v<R>);
        static_assert(    std::is_trivially_move_constructible_v<R>);
        static_assert(not std::is_move_assignable_v<R>);
        static_assert(    std::equality_comparable<R>);
}

BOOST_AUTO_TEST_SUITE_END()
