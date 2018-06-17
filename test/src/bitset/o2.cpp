//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset/compatible/bitset.hpp>         // bitset
#include <bitset/compatible/dynamic_bitset.hpp> // dynamic_bitset
#include <bitset/compatible/int_set.hpp>        // int_set
#include <bitset/exhaustive.hpp>                // all_values, all_singleton_sets, all_singleton_set_pairs,
                                                // all_doubleton_arrays, all_doubleton_ilists, all_doubleton_sets,
#include <bitset/primitives.hpp>                // constructor, const_reference, const_iterator,
                                                // mem_all_of, mem_any_of, mem_none_of, mem_for_each, mem_reverse_for_each,
                                                // op_bitand_assign, op_bitor_assign, op_xor_assign, op_minus_assign,
                                                // op_shift_left_assign, op_shift_right_assign, mem_insert, mem_erase, fn_swap, op_compl,
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

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using BitSetTypes = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset< 32>
,       std::bitset< 64>
,       boost::dynamic_bitset<>
,       int_set<  0, uint32_t>
,       int_set<  1, uint32_t>
,       int_set< 33, uint32_t>
,       int_set< 65, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(BitSet, T, BitSetTypes)
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

        all_singleton_set_pairs<T>(op_less{});
        all_singleton_set_pairs<T>(op_greater{});
        all_singleton_set_pairs<T>(op_greater_equal{});
        all_singleton_set_pairs<T>(op_less_equal{});

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
