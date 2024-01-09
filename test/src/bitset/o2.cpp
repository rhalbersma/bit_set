//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/xstd/bitset.hpp>          // bitset
#include <bitset/exhaustive.hpp>        // all_singleton_sets, all_singleton_set_pairs, any_value
#include <bitset/primitives.hpp>        // op_bit_and_assign, op_bit_or_assign, op_bit_xor_assign, op_minus_assign,
                                        // op_shift_left_assign, op_shift_right_assign, op_bit_not, op_equal_to,
                                        // op_shift_left, op_shift_right, op_bit_and, op_bit_or, op_bit_xor, op_minus,
                                        // mem_is_subset_of, mem_is_proper_subset_of, mem_intersect
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset<0>
,       std::bitset<8>
,       boost::dynamic_bitset<>
,       bitset< 0, uint8_t>
,       bitset< 8, uint8_t>
,       bitset< 9, uint8_t>
,       bitset<17, uint8_t>
,       bitset< 8, uint16_t>
,       bitset< 8, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bitset< 8, uint64_t>
#endif
#if defined(__GNUG__)
,       bitset< 8, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(BitSet, T, bitset_types)
{
        all_singleton_set_pairs<T>(op_bit_and_assign());
        all_singleton_set_pairs<T>(op_bit_or_assign());
        all_singleton_set_pairs<T>(op_bit_xor_assign());
        all_singleton_set_pairs<T>(op_minus_assign());

        any_value<T>([](auto pos) {
                all_singleton_sets<T>([&](auto& bs1) {
                        op_shift_left_assign()(bs1, pos);
                });
        });
        any_value<T>([](auto pos) {
                all_singleton_sets<T>([&](auto& bs1) {
                        op_shift_right_assign()(bs1, pos);
                });
        });

        all_singleton_set_pairs<T>(op_equal_to());

        any_value<T>([](auto pos) {
                all_singleton_sets<T>([&](auto const& bs1) {
                        op_shift_left()(bs1, pos);
                });
        });
        any_value<T>([](auto pos) {
                all_singleton_sets<T>([&](auto const& bs1) {
                        op_shift_right()(bs1, pos);
                });
        });

        all_singleton_set_pairs<T>(op_bit_and());
        all_singleton_set_pairs<T>(op_bit_or());
        all_singleton_set_pairs<T>(op_bit_xor());
        all_singleton_set_pairs<T>(op_minus());

        all_singleton_set_pairs<T>(mem_is_subset_of());
        all_singleton_set_pairs<T>(mem_is_proper_subset_of());
        all_singleton_set_pairs<T>(mem_intersects());
}

BOOST_AUTO_TEST_SUITE_END()
