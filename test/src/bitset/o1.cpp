//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset/compatible/bitset.hpp>         // bitset
#include <bitset/compatible/dynamic_bitset.hpp> // dynamic_bitset
#include <bitset/compatible/int_set.hpp>        // int_set
#include <bitset/exhaustive.hpp>                // all_values, all_cardinality_sets, all_singleton_arrays, all_singleton_ilists, all_singleton_sets
#include <bitset/primitives.hpp>                // constructor, const_reference, const_iterator, mem_front, mem_back,
                                                // mem_accumulate, mem_for_each, mem_reverse_for_each,
                                                // fn_fill, fn_insert, mem_insert, fn_clear, fn_erase, mem_erase,
                                                // op_compl, fn_complement, fn_replace, fn_size, fn_max_size,
                                                // op_equal_to, op_not_equal_to, op_less, op_greater, op_greater_equal, op_less_equal,
                                                // fn_contains, fn_full, fn_not_empty, fn_empty, op_at,
                                                // op_bitand, op_bitor, op_xor, op_minus
                                                // fn_is_subset_of, fn_is_superset_of, fn_is_proper_subset_of, fn_is_proper_superset_of,
                                                // fn_intersects, fn_disjoint,
#include <xstd/int_set.hpp>                     // int_set
#include <boost/dynamic_bitset.hpp>             // dynamic_bitset
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <bitset>                               // bitset

BOOST_AUTO_TEST_SUITE(Linear)

using namespace xstd;

using BitSetTypes = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset< 32>
,       std::bitset< 64>
,       std::bitset< 96>
,       std::bitset<128>
,       boost::dynamic_bitset<>
// ,       int_set<  0, uint32_t>
// ,       int_set<  1, uint32_t>
// ,       int_set<  2, uint32_t>
// ,       int_set< 32, uint32_t>
// ,       int_set< 33, uint32_t>
// ,       int_set< 34, uint32_t>
// ,       int_set< 64, uint32_t>
// ,       int_set< 65, uint32_t>
// ,       int_set< 66, uint32_t>
// ,       int_set< 96, uint32_t>
// ,       int_set<128>
// #if defined(__GNUG__)
// ,       int_set<128, __uint128_t>
// ,       int_set<256, __uint128_t>
// #endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(BitSet, T, BitSetTypes)
{
        all_cardinality_sets<T>(fn_fill{});
        all_singleton_sets<T>(fn_fill{});
        all_cardinality_sets<T>(fn_insert{});
        all_singleton_sets<T>(fn_insert{});
        // all_values<T>([](auto const pos) {
        //         fn_insert{}(T{}, pos);
        //         fn_insert{}(T{}, pos, true);
        //         fn_insert{}(~T{}, pos, false);
        // });

        all_cardinality_sets<T>(fn_clear{});
        all_singleton_sets<T>(fn_clear{});
        all_cardinality_sets<T>(fn_erase{});
        all_singleton_sets<T>(fn_erase{});
        // all_values<T>([](auto const pos) {
        //         fn_erase{}(~T{}, pos);
        // });

        all_cardinality_sets<T>(op_compl{});
        all_singleton_sets<T>(op_compl{});
        all_cardinality_sets<T>(fn_complement{});
        all_singleton_sets<T>(fn_complement{});
        all_cardinality_sets<T>(fn_replace{});
        all_singleton_sets<T>(fn_replace{});
        // all_values<T>([](auto const pos) {
        //         fn_replace{}(T{}, pos);
        //         fn_replace{}(~T{}, pos);
        // });

        all_cardinality_sets<T>(fn_size{});

        all_cardinality_sets<T>(fn_max_size{});
        all_singleton_sets<T>(fn_max_size{});

        all_singleton_sets<T>(fn_contains{});
        // all_values<T>([](auto const pos) {
        //         fn_contains{}(T{}, pos);
        //         fn_contains{}(~T{}, pos);
        // });

        all_cardinality_sets<T>(fn_full{});
        all_cardinality_sets<T>(fn_not_empty{});
        all_cardinality_sets<T>(fn_empty{});

        // all_values<T>([](auto const pos) {
        //         op_at{}(T{}, pos);
        //         op_at{}(~T{}, pos);
        //         auto none = T{};
        //         op_at{}(none, pos);
        //         op_at{}(none, pos, true);
        //         op_at{}(none, pos, false);
        //         auto ones = ~T{};
        //         op_at{}(ones, pos);
        //         op_at{}(ones, pos, false);
        //         op_at{}(ones, pos, true);
        // });

        all_singleton_sets<T>(op_bitand{});
        all_singleton_sets<T>(op_bitor{});
        all_singleton_sets<T>(op_xor{});
        all_singleton_sets<T>(op_minus{});

        all_cardinality_sets<T>(fn_is_subset_of{});
        all_singleton_sets<T>(fn_is_subset_of{});

        all_cardinality_sets<T>(fn_intersects{});
        all_singleton_sets<T>(fn_intersects{});
}

BOOST_AUTO_TEST_SUITE_END()
