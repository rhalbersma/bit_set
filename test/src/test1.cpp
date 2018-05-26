//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>                               // bitset
#include <xstd/int_set.hpp>                     // int_set
#include <legacy.hpp>                           // bitset, int_set
#include <exhaustive.hpp>                       // all_values, all_cardinality_sets, all_singleton_arrays, all_singleton_ilists, all_singleton_sets
#include <primitive.hpp>                        // constructor, mem_assign, const_reference, const_iterator, mem_front, mem_back,
                                                // mem_accumulate, mem_for_each, mem_reverse_for_each, fn_set, mem_insert, fn_reset, mem_erase,
                                                // op_compl, fn_flip, mem_size, mem_max_size, op_equal_to, op_less, fn_intersects, fn_is_subset_of,
                                                // op_not_equal_to, op_greater, op_greater_equal, op_less_equal,
                                                // fn_disjoint, fn_is_superset_of, fn_is_proper_subset_of, fn_is_proper_superset_of,
                                                // fn_test, fn_all, fn_any, fn_none, op_at, op_bitand, op_bitor, op_xor, op_minus
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Linear)

using namespace xstd;

using SetTypes = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset< 32>
,       std::bitset< 64>
,       std::bitset< 96>
,       std::bitset<128>
,       int_set<  0, uint32_t>
,       int_set<  1, uint32_t>
,       int_set<  2, uint32_t>
,       int_set< 32, uint32_t>
,       int_set< 33, uint32_t>
,       int_set< 34, uint32_t>
,       int_set< 64, uint32_t>
,       int_set< 65, uint32_t>
,       int_set< 66, uint32_t>
,       int_set< 96, uint32_t>
,       int_set<128>
#if defined(__GNUG__)
,       int_set<128, __uint128_t>
,       int_set<256, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        all_singleton_arrays<T>([](auto const& a1) {
                constructor<T>{}(a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                constructor<T>{}(ilist1);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                mem_assign{}(~T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                mem_assign{}(~T{}, ilist1);
        });

        all_cardinality_sets<T>(const_reference{});
        all_singleton_sets<T>(const_reference{});

        all_cardinality_sets<T>(const_iterator{});
        all_singleton_sets<T>(const_iterator{});

        all_cardinality_sets<T>(mem_front{});
        all_singleton_sets<T>(mem_front{});

        all_cardinality_sets<T>(mem_back{});
        all_singleton_sets<T>(mem_back{});

        all_cardinality_sets<T>([](auto const& in) {
                mem_accumulate{}(in, 0);
        });
        all_singleton_sets<T>([](auto const& i1) {
                mem_accumulate{}(i1, 0);
        });

        all_cardinality_sets<T>(mem_for_each{});
        all_singleton_sets<T>(mem_for_each{});

        all_cardinality_sets<T>(mem_reverse_for_each{});
        all_singleton_sets<T>(mem_reverse_for_each{});

        all_singleton_sets<T>(fn_set{});
        all_values<T>([](auto const pos) {
                fn_set{}(T{}, pos);
                fn_set{}(T{}, pos, true);
                fn_set{}(T{}, pos, false);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                mem_insert{}(T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                mem_insert{}(T{}, ilist1);
        });

        all_singleton_sets<T>(fn_reset{});
        all_values<T>([](auto const pos) {
                fn_reset{}(~T{}, pos);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                mem_erase{}(~T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                mem_erase{}(T{}, ilist1);
        });

        all_singleton_sets<T>(op_compl{});
        all_singleton_sets<T>(fn_flip{});
        all_values<T>([](auto const pos) {
                fn_flip{}(T{}, pos);
        });

        all_cardinality_sets<T>(mem_size{});
        all_singleton_sets<T>(mem_max_size{});

        all_cardinality_sets<T>(op_equal_to{});
        all_cardinality_sets<T>(op_less{});
        all_cardinality_sets<T>(fn_intersects{});
        all_cardinality_sets<T>(fn_is_subset_of{});

        all_singleton_sets<T>(op_equal_to{});
        all_singleton_sets<T>(op_not_equal_to{});

        all_singleton_sets<T>(op_less{});
        all_singleton_sets<T>(op_greater{});
        all_singleton_sets<T>(op_greater_equal{});
        all_singleton_sets<T>(op_less_equal{});

        all_singleton_sets<T>(fn_intersects{});
        all_singleton_sets<T>(fn_disjoint{});

        all_singleton_sets<T>(fn_is_subset_of{});
        all_singleton_sets<T>(fn_is_superset_of{});
        all_singleton_sets<T>(fn_is_proper_subset_of{});
        all_singleton_sets<T>(fn_is_proper_superset_of{});

        all_values<T>([](auto const pos) {
                fn_test{}(T{}, pos);
        });

        all_cardinality_sets<T>(mem_full{});
        all_cardinality_sets<T>(fn_any{});
        all_cardinality_sets<T>(mem_empty{});

        all_values<T>([](auto const pos) {
                op_at{}(T{}, pos);
        });

        all_singleton_sets<T>(op_bitand{});
        all_singleton_sets<T>(op_bitor{});
        all_singleton_sets<T>(op_xor{});
        all_singleton_sets<T>(op_minus{});
}

BOOST_AUTO_TEST_SUITE_END()
