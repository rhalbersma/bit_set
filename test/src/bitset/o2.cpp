//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset/compatible/bit_set.hpp>        // bit_set
#include <bitset/compatible/bitset.hpp>         // bitset
#include <bitset/compatible/dynamic_bitset.hpp> // dynamic_bitset
#include <bitset/exhaustive.hpp>                // all_singleton_sets, all_singleton_set_pairs, any_value
#include <bitset/primitives.hpp>                // op_bitand_assign, op_bitor_assign, op_xor_assign, op_minus_assign,
                                                // op_shift_left_assign, op_shift_right_assign, op_compl,
                                                // op_equal_to, op_not_equal_to, op_less, op_greater, op_greater_equal, op_less_equal,
                                                // op_shift_left, op_shift_right, op_bitand, op_bitor, op_xor, op_minus,
                                                // mem_is_subset_of, mem_is_superset_of, mem_is_proper_subset_of, mem_is_proper_superset_of,
                                                // mem_intersect, fn_is_disjoint
#include <xstd/bit_set.hpp>                     // bit_set
#include <boost/dynamic_bitset.hpp>             // dynamic_bitset
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <bitset>                               // bitset
#include <cstdint>                              // uint8_t, uint16_t, uint32_t

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset<0>
,       std::bitset<8>
,       boost::dynamic_bitset<>
,       bit_set< 0, uint8_t>
,       bit_set< 8, uint8_t>
,       bit_set< 9, uint8_t>
,       bit_set<17, uint8_t>
,       bit_set< 8, uint16_t>
,       bit_set< 8, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bit_set< 8, uint64_t>
#endif
#if defined(__GNUG__)
,       bit_set< 8, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(BitSet, T, bitset_types)
{
        all_singleton_set_pairs<T>(op_bitand_assign());
        all_singleton_set_pairs<T>(op_bitor_assign());
        all_singleton_set_pairs<T>(op_xor_assign());
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

        all_singleton_set_pairs<T>(op_compl());

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

        all_singleton_set_pairs<T>(op_bitand());
        all_singleton_set_pairs<T>(op_bitor());
        all_singleton_set_pairs<T>(op_xor());
        all_singleton_set_pairs<T>(op_minus());

        all_singleton_set_pairs<T>(mem_is_subset_of());
        all_singleton_set_pairs<T>(mem_is_superset_of());
        all_singleton_set_pairs<T>(mem_is_proper_subset_of());
        all_singleton_set_pairs<T>(mem_is_proper_superset_of());

        all_singleton_set_pairs<T>(mem_intersects());
        all_singleton_set_pairs<T>(fn_is_disjoint());
}

BOOST_AUTO_TEST_SUITE_END()
