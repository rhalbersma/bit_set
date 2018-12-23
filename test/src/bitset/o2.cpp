//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <bitset/compatible/bitset.hpp>         // bitset
#include <bitset/compatible/dynamic_bitset.hpp> // dynamic_bitset
#include <bitset/compatible/int_set.hpp>        // int_set
#include <bitset/exhaustive.hpp>                // all_values, all_singleton_sets, all_singleton_set_pairs
#include <bitset/primitives.hpp>                // op_bitand_assign, op_bitor_assign, op_xor_assign, op_minus_assign,
                                                // op_shift_left_assign, op_shift_right_assign, op_compl,
                                                // op_equal_to, op_not_equal_to, op_less, op_greater, op_greater_equal, op_less_equal,
                                                // op_shift_left, op_shift_right, op_bitand, op_bitor, op_xor, op_minus,
                                                // fn_is_subset_of, fn_is_superset_of, fn_is_proper_subset_of, fn_is_proper_superset_of,
                                                // fn_intersect, fn_disjoint
#include <xstd/int_set.hpp>                     // int_set
#include <boost/dynamic_bitset.hpp>             // dynamic_bitset
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <bitset>                               // bitset
#include <cstdint>                              // uint8_t, uint16_t, uint32_t

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset<  1>
,       std::bitset< 31>
,       std::bitset< 32>
,       std::bitset< 33>
,       std::bitset< 63>
,       std::bitset< 64>
,       std::bitset< 65>
,       boost::dynamic_bitset<>
,       int_set<  0, uint8_t>
,       int_set<  1, uint8_t>
,       int_set<  7, uint8_t>
,       int_set<  8, uint8_t>
,       int_set<  9, uint8_t>
,       int_set< 15, uint8_t>
,       int_set< 16, uint8_t>
,       int_set< 17, uint8_t>
,       int_set<  0, uint16_t>
,       int_set<  1, uint16_t>
,       int_set< 15, uint16_t>
,       int_set< 16, uint16_t>
,       int_set< 17, uint16_t>
,       int_set< 31, uint16_t>
,       int_set< 32, uint16_t>
,       int_set< 33, uint16_t>
,       int_set<  0, uint32_t>
,       int_set<  1, uint32_t>
,       int_set< 31, uint32_t>
,       int_set< 32, uint32_t>
,       int_set< 33, uint32_t>
,       int_set< 63, uint32_t>
,       int_set< 64, uint32_t>
,       int_set< 65, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(BitSet, T, bitset_types)
{
        all_singleton_set_pairs<T>(op_bitand_assign{});
        all_singleton_set_pairs<T>(op_bitor_assign{});
        all_singleton_set_pairs<T>(op_xor_assign{});
        all_singleton_set_pairs<T>(op_minus_assign{});

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto& i1){
                        op_shift_left_assign{}(i1, pos);
                });
        });

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto& i1){
                        op_shift_right_assign{}(i1, pos);
                });
        });

        all_singleton_set_pairs<T>(op_compl{});

        all_singleton_set_pairs<T>(op_equal_to{});
        all_singleton_set_pairs<T>(op_not_equal_to{});

        all_values<T>([](auto const pos){
                all_singleton_sets<T>([&](auto const& i1){
                        op_shift_left{}(i1, pos);
                });
        });

        all_values<T>([](auto const pos){
                all_singleton_sets<T>([&](auto const& i1){
                        op_shift_right{}(i1, pos);
                });
        });

        all_singleton_set_pairs<T>(op_bitand{});
        all_singleton_set_pairs<T>(op_bitor{});
        all_singleton_set_pairs<T>(op_xor{});
        all_singleton_set_pairs<T>(op_minus{});

        all_singleton_set_pairs<T>(fn_is_subset_of{});
        all_singleton_set_pairs<T>(fn_is_superset_of{});
        all_singleton_set_pairs<T>(fn_is_proper_subset_of{});
        all_singleton_set_pairs<T>(fn_is_proper_superset_of{});

        all_singleton_set_pairs<T>(fn_intersects{});
        all_singleton_set_pairs<T>(fn_disjoint{});
}

BOOST_AUTO_TEST_SUITE_END()
