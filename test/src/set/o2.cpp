//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <set/compatible/flat_set.hpp>  // op<=>
#include <set/compatible/set.hpp>       // op<=>
#include <set/exhaustive.hpp>           // all_values, all_singleton_sets, all_singleton_set_pairs,
                                        // all_doubleton_arrays, all_doubleton_ilists, all_doubleton_sets,
#include <set/primitives.hpp>           // constructor, op_assign, mem_insert, mem_erase, mem_swap, mem_find, mem_count,
                                        // mem_lower_bound, mem_upper_bound, mem_equal_range, op_equal, op_not_equal_to,
                                        // op_compare_three_way op_less, op_greater, op_less_equal, op_greater_equal, fn_swap
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using int_set_types = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       bit_set< 0, uint8_t>
,       bit_set< 1, uint8_t>
,       bit_set< 7, uint8_t>
,       bit_set< 8, uint8_t>
,       bit_set< 9, uint8_t>
,       bit_set<15, uint8_t>
,       bit_set<16, uint8_t>
,       bit_set<17, uint8_t>
,       bit_set<24, uint8_t>
,       bit_set< 0, uint16_t>
,       bit_set< 1, uint16_t>
,       bit_set<15, uint16_t>
,       bit_set<16, uint16_t>
,       bit_set<17, uint16_t>
,       bit_set<31, uint16_t>
,       bit_set<32, uint16_t>
,       bit_set<33, uint16_t>
,       bit_set<48, uint16_t>
,       bit_set< 0, uint32_t>
,       bit_set< 1, uint32_t>
,       bit_set<31, uint32_t>
,       bit_set<32, uint32_t>
,       bit_set<33, uint32_t>
,       bit_set<63, uint32_t>
,       bit_set<64, uint32_t>
,       bit_set<65, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IntSet, T, int_set_types)
{
        all_doubleton_arrays<T>([](auto const& a2) {
                constructor<T>()(a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                constructor<T>()(ilist2);
        });

        all_singleton_sets<T>([](auto& is1) {
                all_singleton_ilists<T>([&](auto ilist1) {
                        op_assign()(is1, ilist1);
                });
        });

        all_doubleton_arrays<T>([](auto const& a2) {
                empty_set<T>([&](auto& is0) {
                        mem_insert()(is0, a2.begin(), a2.end());
                });
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                empty_set<T>([=](auto& is0) {
                        mem_insert()(is0, ilist2);
                });
        });

        // boost::container::flat_set<int>::erase invalidates iterators
        if constexpr (!std::is_same_v<T, boost::container::flat_set<int>>) {
                all_doubleton_sets<T>([](auto& is2) {
                        mem_erase()(is2, is2.begin(), is2.end());
                });
        }

        all_singleton_set_pairs<T>(mem_swap());

        all_values<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto& is1) {
                        mem_find()(is1, x);
                });
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_find()(is1, x);
                });
        });

        all_values<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_count()(is1, x);
                });
        });

        // std::set and boost::container::flat_set do not yet support contains() (added for C++2a)
        if constexpr (!(std::is_same_v<T, std::set<int>> || std::is_same_v<T, boost::container::flat_set<int>>)) {
                all_values<T>([](auto const& x) {
                        all_singleton_sets<T>([&](auto const& is1) {
                                mem_contains()(is1, x);
                        });
                });
        }

        all_values<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto& is1) {
                        mem_lower_bound()(is1, x);
                });
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_lower_bound()(is1, x);
                });
        });

        all_values<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto& is1) {
                        mem_upper_bound()(is1, x);
                });
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_upper_bound()(is1, x);
                });
        });

        all_values<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto& is1) {
                        mem_equal_range()(is1, x);
                });
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_equal_range()(is1, x);
                });
        });

        all_singleton_set_pairs<T>(op_equal_to());
        all_singleton_set_pairs<T>(op_not_equal_to());

        all_singleton_set_pairs<T>(op_compare_three_way());
        all_singleton_set_pairs<T>(op_less());
        all_singleton_set_pairs<T>(op_greater());
        all_singleton_set_pairs<T>(op_less_equal());
        all_singleton_set_pairs<T>(op_greater_equal());

        all_singleton_set_pairs<T>(fn_swap());
}

BOOST_AUTO_TEST_SUITE_END()
