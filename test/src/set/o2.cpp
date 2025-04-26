//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/container/flat_set.hpp>     // flat_set
#include <set/composable.hpp>                   // includes, set_difference, set_intersection, set_symmetric_difference, set_union,
                                                // decrement, increment
#include <set/exhaustive.hpp>                   // all_doubleton_arrays, all_doubleton_ilists, all_doubleton_sets,
                                                // all_singleton_sets, all_singleton_set_pairs, all_valid
#include <set/primitives.hpp>                   // constructor, op_assign, mem_insert, mem_erase, mem_swap, mem_find, mem_count,
                                                // mem_lower_bound, mem_upper_bound, mem_equal_range, op_equal, op_not_equal_to,
                                                // op_compare_three_way op_less, op_greater, op_less_equal, op_greater_equal, fn_swap
#include <xstd/bit_set.hpp>                     // bit_set
#include <boost/mp11/list.hpp>                  // mp_list
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <concepts>                             // same_as
#include <cstddef>                              // size_t
#include <cstdint>                              // uint8_t, uint16_t, uint32_t
#include <set>                                  // set

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using Types = boost::mp11::mp_list
<       std::set<std::size_t>
,       boost::container::flat_set<std::size_t>
,       bit_set< 0, uint8_t>
,       bit_set< 1, uint8_t>
,       bit_set< 8, uint8_t>
,       bit_set< 9, uint8_t>
,       bit_set<16, uint8_t>
,       bit_set<17, uint8_t>
,       bit_set<24, uint8_t>
,       bit_set<24, uint16_t>
,       bit_set<24, uint32_t>
,       bit_set<24, uint64_t>
#if defined(__GNUG__)
,       bit_set<24, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IntSet, T, Types)
{
        all_doubleton_arrays<T>([](auto const& a2) {
                constructor<T>()(a2.begin(), a2.end());
                // boost::container::flat_set<std::size_t> does not have from_range constructor
                if constexpr (not std::same_as<T, boost::container::flat_set<std::size_t>>) {
                        constructor<T>()(std::from_range, a2);
                }
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                // boost::container::flat_set<std::size_t> does not have from_range constructor
                if constexpr (not std::same_as<T, boost::container::flat_set<std::size_t>>) {
                        constructor<T>()(std::from_range, ilist2);
                }
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
                full_set<T>([&](auto& isN) {
                        mem_insert()(isN, a2.begin(), a2.end());
                });
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                empty_set<T>([=](auto& is0) {
                        mem_insert()(is0, ilist2);
                });
                full_set<T>([=](auto& isN) {
                        mem_insert()(isN, ilist2);
                });
        });

        // boost::container::flat_set<std::size_t>::erase invalidates iterators
        if constexpr (not std::same_as<T, boost::container::flat_set<std::size_t>>) {
                all_doubleton_sets<T>([](auto& is2) {
                        mem_erase()(is2, is2.begin(), is2.end());
                });
        }

        all_singleton_set_pairs<T>(op_equal_to());
        all_singleton_set_pairs<T>(mem_swap());
        all_singleton_set_pairs<T>(fn_swap());

        all_valid<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto& is1) {
                        mem_find()(is1, x);
                });
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_find()(is1, x);
                });
        });

        all_valid<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_count()(is1, x);
                });
        });

        all_valid<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_contains()(is1, x);
                });
        });

        all_valid<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto& is1) {
                        mem_lower_bound()(is1, x);
                });
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_lower_bound()(is1, x);
                });
        });

        all_valid<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto& is1) {
                        mem_upper_bound()(is1, x);
                });
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_upper_bound()(is1, x);
                });
        });

        all_valid<T>([](auto const& x) {
                all_singleton_sets<T>([&](auto& is1) {
                        mem_equal_range()(is1, x);
                });
                all_singleton_sets<T>([&](auto const& is1) {
                        mem_equal_range()(is1, x);
                });
        });

        all_singleton_set_pairs<T>(op_not_equal_to());

        all_singleton_set_pairs<T>(op_compare_three_way());
        all_singleton_set_pairs<T>(op_less());
        all_singleton_set_pairs<T>(op_greater());
        all_singleton_set_pairs<T>(op_less_equal());
        all_singleton_set_pairs<T>(op_greater_equal());

        all_singleton_set_pairs<T>(composable::includes());
        all_singleton_set_pairs<T>(composable::set_union());
        all_singleton_set_pairs<T>(composable::set_intersection());
        all_singleton_set_pairs<T>(composable::set_difference());
        all_singleton_set_pairs<T>(composable::set_symmetric_difference());

        all_valid<T>([](auto pos) {
                all_singleton_sets<T>([&](auto const& bs1) {
                        composable::increment_modulo()(bs1, static_cast<std::size_t>(pos));
                });
        });
        all_valid<T>([](auto pos) {
                all_singleton_sets<T>([&](auto const& bs1) {
                        composable::decrement_modulo()(bs1, static_cast<std::size_t>(pos));
                });
        });
}

BOOST_AUTO_TEST_SUITE_END()
