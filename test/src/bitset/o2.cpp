//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/xstd/bitset.hpp>          // bitset
#include <bitset/exhaustive.hpp>        // all_singleton_sets, all_singleton_set_pairs, any_value
#include <bitset/primitives.hpp>        // mem_bit_and_assign, mem_bit_or_assign, mem_bit_xor_assign, mem_bit_minus_assign,
                                        // mem_shift_left_assign, mem_shift_right_assign, mem_shift_left, mem_shift_right,
                                        // mem_equal_to, mem_compare_three_way, mem_is_subset_of, mem_is_proper_subset_of, mem_intersects,
                                        // op_bit_and, op_bit_or, op_bit_xor, op_bit_minus,
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Quadratic)

using Types = boost::mp11::mp_list
<       boost::dynamic_bitset<>
,         std::bitset<0>
,         std::bitset<8>
,        xstd::bitset< 0, uint8_t>
,        xstd::bitset< 8, uint8_t>
,        xstd::bitset< 9, uint8_t>
,        xstd::bitset<17, uint8_t>
,        xstd::bitset< 8, uint16_t>
,        xstd::bitset< 8, uint32_t>
,        xstd::bitset< 8, uint64_t>
#if defined(__GNUG__)
,        xstd::bitset< 8, __uint128_t>
#endif
>;

using namespace xstd;

BOOST_AUTO_TEST_CASE_TEMPLATE(Operations, T, Types)
{
        on2::all_singleton_set_pairs<T>(mem_bit_and_assign());
        on2::all_singleton_set_pairs<T>(mem_bit_or_assign());
        on2::all_singleton_set_pairs<T>(mem_bit_xor_assign());
        on2::all_singleton_set_pairs<T>(mem_bit_minus_assign());

        on1::any_value<T>([](auto pos) {
                on1::all_singleton_sets<T>([&](auto& bs1) {
                        mem_shift_left_assign()(bs1, pos);
                });
        });
        on1::any_value<T>([](auto pos) {
                on1::all_singleton_sets<T>([&](auto& bs1) {
                        mem_shift_right_assign()(bs1, pos);
                });
        });

        on1::any_value<T>([](auto pos) {
                on1::all_singleton_sets<T>([&](auto const& bs1) {
                        mem_shift_left()(bs1, pos);
                });
        });
        on1::any_value<T>([](auto pos) {
                on1::all_singleton_sets<T>([&](auto const& bs1) {
                        mem_shift_right()(bs1, pos);
                });
        });
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Observers, T, Types)
{
        on2::all_singleton_set_pairs<T>(mem_equal_to());
        on2::all_singleton_set_pairs<T>(mem_compare_three_way());
        on2::all_singleton_set_pairs<T>(mem_is_subset_of());
        on2::all_singleton_set_pairs<T>(mem_is_proper_subset_of());
        on2::all_singleton_set_pairs<T>(mem_intersects());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Operators, T, Types)
{
        on2::all_singleton_set_pairs<T>(op_bit_and());
        on2::all_singleton_set_pairs<T>(op_bit_or());
        on2::all_singleton_set_pairs<T>(op_bit_xor());
        on2::all_singleton_set_pairs<T>(op_bit_minus());
}

BOOST_AUTO_TEST_SUITE_END()
