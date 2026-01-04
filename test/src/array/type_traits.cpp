//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_array.hpp>           // bit_array
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <array>                        // array
#include <concepts>                     // equality_comparable, regular, totally_ordered
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <iterator>                     // random_access_iterator
#include <ranges>                       // random_access_range
#include <type_traits>                  // is_default_xxx_v, is_reference_v, is_trivially_xxx_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

using namespace xstd;

using Types = boost::mp11::mp_list
<       std::array<bool, 0>
,       std::array<bool, 8>
,       bit_array<  0, uint8_t>
,       bit_array<  8, uint8_t>
,       bit_array< 16, uint8_t>
,       bit_array< 24, uint8_t>
,       bit_array<  0, uint16_t>
,       bit_array< 16, uint16_t>
,       bit_array< 32, uint16_t>
,       bit_array< 48, uint16_t>
,       bit_array<  0, uint32_t>
,       bit_array< 32, uint32_t>
,       bit_array< 64, uint32_t>
,       bit_array< 96, uint32_t>
,       bit_array<  0, uint64_t>
,       bit_array< 64, uint64_t>
,       bit_array<128, uint64_t>
,       bit_array<192, uint64_t>
#if defined(__GNUG__)
,       bit_array<  0, __uint128_t>
,       bit_array<128, __uint128_t>
,       bit_array<256, __uint128_t>
,       bit_array<384, __uint128_t>
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
