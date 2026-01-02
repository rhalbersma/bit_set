//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <set/exhaustive.hpp>           // all_cardinality_sets, all_singleton_arrays, all_singleton_ilists, all_singleton_sets,
                                        // all_valid, empty_set, full_set
#include <set/primitives.hpp>           // constructor, mem_const_reference, mem_const_iterator, mem_front, mem_back,
                                        // mem_empty, mem_size, mem_max_size, mem_insert, mem_erase, mem_clear,
                                        // op_equal, op_compare_three_way, fn_iterator, fn_size, fn_ssize, fn_empty
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <flat_set>                     // flat_set
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Linear)

using namespace xstd;

using Types = boost::mp11::mp_list
<       std::set<std::size_t>
,       std::flat_set<std::size_t>
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
        on1::all_singleton_arrays<T>([](auto const& a1) {
                constructor<T>()(a1.begin(), a1.end());
                constructor<T>()(std::from_range, a1);
        });
        on1::all_singleton_ilists<T>([](auto ilist1) {
                constructor<T>()(std::from_range, ilist1);
                constructor<T>()(ilist1);
        });

        on1::all_cardinality_sets<T>(mem_const_reference());
        on1::all_singleton_sets<T>(mem_const_reference());

        on1::all_cardinality_sets<T>([](auto& is) {
                mem_const_iterator()(is);
        });
        on1::all_cardinality_sets<T>([](auto const& is) {
                mem_const_iterator()(is);
        });
        on1::all_singleton_sets<T>([](auto& is1) {
                mem_const_iterator()(is1);
        });
        on1::all_singleton_sets<T>([](auto const& is1) {
                mem_const_iterator()(is1);
        });

        on1::all_cardinality_sets<T>(mem_front());
        on1::all_singleton_sets<T>(mem_front());

        on1::all_cardinality_sets<T>(mem_back());
        on1::all_singleton_sets<T>(mem_back());

        on1::all_cardinality_sets<T>(mem_empty());
        on1::all_cardinality_sets<T>(mem_size());
        on1::all_cardinality_sets<T>(mem_max_size());

        on1::all_valid<T>([](auto const& t) {
                on0::empty_set<T>([=](auto& is0) {
                        mem_emplace()(is0, t);
                });
                on0::full_set<T>([=](auto& isN) {
                        mem_emplace()(isN, t);
                });
                on0::empty_set<T>([=](auto& is0) {
                        mem_insert()(is0, t);
                });
                on0::full_set<T>([=](auto& isN) {
                        mem_insert()(isN, t);
                });
        });
        on1::all_valid<T>([](auto const& t) {
                on0::empty_set<T>([=](auto& is0) {
                        mem_emplace_hint()(is0, is0.end(), t);
                });
                on0::full_set<T>([=](auto& isN) {
                        mem_emplace_hint()(isN, isN.end(), t);
                });
                on0::empty_set<T>([=](auto& is0) {
                        mem_insert()(is0, is0.end(), t);
                });
                on0::full_set<T>([=](auto& isN) {
                        mem_emplace_hint()(isN, isN.end(), t);
                });
        });
        on1::all_singleton_arrays<T>([](auto const& a1) {
                on0::empty_set<T>([&](auto& is0) {
                        mem_insert()(is0, a1.begin(), a1.end());
                });
                on0::full_set<T>([&](auto& isN) {
                        mem_insert()(isN, a1.begin(), a1.end());
                });
        });
        on1::all_singleton_ilists<T>([](auto ilist1) {
                on0::empty_set<T>([&](auto& is0) {
                        mem_insert()(is0, ilist1);
                });
                on0::full_set<T>([&](auto& isN) {
                        mem_insert()(isN, ilist1);
                });
        });

        on1::all_valid<T>([](auto const& k) {
                on0::empty_set<T>([&](auto& is0) {
                        mem_erase()(is0, k);
                });
                on0::full_set<T>([&](auto& isN) {
                        mem_erase()(isN, k);
                });
        });

        // std:flat_set<std::size_t>::erase invalidates iterators
        if constexpr (not std::same_as<T, std::flat_set<std::size_t>>) {
                on0::full_set<T>([](auto& isN) {
                        for (auto first = isN.begin(), last = isN.end(); first != last; /* expression inside loop */) {
                                mem_erase()(isN, first++);
                        }
                });
                on0::full_set<T>([](auto& isN) {
                        mem_erase()(isN, isN.begin(), isN.end());
                });
        }

        on1::all_cardinality_sets<T>(mem_clear());
        on1::all_singleton_sets<T>(mem_clear());

        on1::all_cardinality_sets<T>(op_compare_three_way());
        on1::all_singleton_sets<T>(op_compare_three_way());

        on1::all_cardinality_sets<T>([](auto& is) {
                fn_iterator()(is);
        });
        on1::all_cardinality_sets<T>([](auto const& is) {
                fn_iterator()(is);
        });

        on1::all_cardinality_sets<T>(fn_size());
        on1::all_cardinality_sets<T>(fn_ssize());
        on1::all_cardinality_sets<T>(fn_empty());
}

BOOST_AUTO_TEST_SUITE_END()
