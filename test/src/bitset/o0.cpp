//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/xstd/bitset.hpp>          // bitset
#include <bitset/exhaustive.hpp>        // empty_set_pair
#include <bitset/primitives.hpp>        // constructor,
                                        // mem_bit_and_assign, mem_bit_or_assign, mem_bit_xor_assign, mem_bit_minus_assign,
                                        // mem_equal_to, mem_compare_three_way, mem_is_subset_of, mem_is_proper_subset_of, mem_intersect,
                                        // op_bit_and, op_bit_or, op_bit_xor, op_bit_minus,
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Constant)

using Types = boost::mp11::mp_list
<       boost::dynamic_bitset<>
,         std::bitset<  0>
,         std::bitset<  1>
,         std::bitset< 31>
,         std::bitset< 32>
,         std::bitset< 33>
,         std::bitset< 63>
,         std::bitset< 64>
,         std::bitset< 65>
,        xstd::bitset<  0, uint8_t>
,        xstd::bitset<  1, uint8_t>
,        xstd::bitset<  7, uint8_t>
,        xstd::bitset<  8, uint8_t>
,        xstd::bitset<  9, uint8_t>
,        xstd::bitset< 15, uint8_t>
,        xstd::bitset< 16, uint8_t>
,        xstd::bitset< 17, uint8_t>
,        xstd::bitset< 24, uint8_t>
,        xstd::bitset<  0, uint16_t>
,        xstd::bitset<  1, uint16_t>
,        xstd::bitset< 15, uint16_t>
,        xstd::bitset< 16, uint16_t>
,        xstd::bitset< 17, uint16_t>
,        xstd::bitset< 31, uint16_t>
,        xstd::bitset< 32, uint16_t>
,        xstd::bitset< 33, uint16_t>
,        xstd::bitset< 48, uint16_t>
,        xstd::bitset<  0, uint32_t>
,        xstd::bitset<  1, uint32_t>
,        xstd::bitset< 31, uint32_t>
,        xstd::bitset< 32, uint32_t>
,        xstd::bitset< 33, uint32_t>
,        xstd::bitset< 63, uint32_t>
,        xstd::bitset< 64, uint32_t>
,        xstd::bitset< 65, uint32_t>
,        xstd::bitset<  0, uint64_t>
,        xstd::bitset<  1, uint64_t>
,        xstd::bitset< 63, uint64_t>
,        xstd::bitset< 64, uint64_t>
,        xstd::bitset< 65, uint64_t>
#if defined(__GNUG__)
,        xstd::bitset<  0, __uint128_t>
,        xstd::bitset<  1, __uint128_t>
,        xstd::bitset<127, __uint128_t>
,        xstd::bitset<128, __uint128_t>
,        xstd::bitset<129, __uint128_t>
#endif
>;

using namespace xstd;

BOOST_AUTO_TEST_CASE_TEMPLATE(Constructors, T, Types)
{
        constructor<T>()();
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Operations, T, Types)
{
        empty_set_pair<T>(mem_bit_and_assign());
        empty_set_pair<T>(mem_bit_or_assign());
        empty_set_pair<T>(mem_bit_xor_assign());
        empty_set_pair<T>(mem_bit_minus_assign());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Observers, T, Types)
{
        empty_set_pair<T>(mem_equal_to());
        empty_set_pair<T>(mem_compare_three_way());
        empty_set_pair<T>(mem_is_subset_of());
        empty_set_pair<T>(mem_is_proper_subset_of());
        empty_set_pair<T>(mem_intersects());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Operators, T, Types)
{
        empty_set_pair<T>(op_bit_and());
        empty_set_pair<T>(op_bit_or());
        empty_set_pair<T>(op_bit_xor());
        empty_set_pair<T>(op_bit_minus());
}

BOOST_AUTO_TEST_SUITE_END()
