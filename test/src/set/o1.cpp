//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <set/compatible/flat_set.hpp>  // op<=>
#include <set/compatible/set.hpp>       // op<=>
#include <set/exhaustive.hpp>           // empty_set, full_set, all_values, all_cardinality_sets,
                                        // all_singleton_arrays, all_singleton_ilists, all_singleton_sets
#include <set/primitives.hpp>           // constructor, mem_const_reference, mem_const_iterator, mem_front, mem_back,
                                        // mem_empty, mem_size, mem_max_size, mem_insert, mem_erase, mem_clear,
                                        // op_equal, op_compare_three_way, fn_iterator, fn_size, fn_ssize, fn_empty
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <set>                          // bitset

BOOST_AUTO_TEST_SUITE(Linear)

using namespace xstd;

using int_set_types = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       bit_set<  0, uint8_t>
,       bit_set<  1, uint8_t>
,       bit_set<  7, uint8_t>
,       bit_set<  8, uint8_t>
,       bit_set<  9, uint8_t>
,       bit_set< 15, uint8_t>
,       bit_set< 16, uint8_t>
,       bit_set< 17, uint8_t>
,       bit_set< 24, uint8_t>
,       bit_set<  0, uint16_t>
,       bit_set<  1, uint16_t>
,       bit_set< 15, uint16_t>
,       bit_set< 16, uint16_t>
,       bit_set< 17, uint16_t>
,       bit_set< 31, uint16_t>
,       bit_set< 32, uint16_t>
,       bit_set< 33, uint16_t>
,       bit_set< 48, uint16_t>
,       bit_set<  0, uint32_t>
,       bit_set<  1, uint32_t>
,       bit_set< 31, uint32_t>
,       bit_set< 32, uint32_t>
,       bit_set< 33, uint32_t>
,       bit_set< 63, uint32_t>
,       bit_set< 64, uint32_t>
,       bit_set< 65, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bit_set<  0, uint64_t>
,       bit_set<  1, uint64_t>
,       bit_set< 63, uint64_t>
,       bit_set< 64, uint64_t>
,       bit_set< 65, uint64_t>
#endif
#if defined(__GNUG__)
,       bit_set<  0, __uint128_t>
,       bit_set<  1, __uint128_t>
,       bit_set<127, __uint128_t>
,       bit_set<128, __uint128_t>
,       bit_set<129, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IntSet, T, int_set_types)
{
        all_singleton_arrays<T>([](auto const& a1) {
                constructor<T>()(a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                constructor<T>()(ilist1);
        });

        all_cardinality_sets<T>(mem_const_reference());
        all_singleton_sets<T>(mem_const_reference());

        all_cardinality_sets<T>([](auto& is) {
                mem_const_iterator()(is);
        });
        all_cardinality_sets<T>([](auto const& is) {
                mem_const_iterator()(is);
        });
        all_singleton_sets<T>([](auto& is1) {
                mem_const_iterator()(is1);
        });
        all_singleton_sets<T>([](auto const& is1) {
                mem_const_iterator()(is1);
        });

        all_cardinality_sets<T>(mem_front());
        all_singleton_sets<T>(mem_front());

        all_cardinality_sets<T>(mem_back());
        all_singleton_sets<T>(mem_back());

        all_cardinality_sets<T>(mem_empty());
        all_cardinality_sets<T>(mem_size());
        all_cardinality_sets<T>(mem_max_size());
        all_cardinality_sets<T>(mem_capacity());

        all_values<T>([](auto const& t) {
                empty_set<T>([=](auto& is0) {
                        mem_emplace()(is0, t);
                });
                empty_set<T>([=](auto& is0) {
                        mem_insert()(is0, t);
                });
        });
        all_values<T>([](auto const& t) {
                empty_set<T>([=](auto& is0) {
                        mem_emplace_hint()(is0, is0.end(), t);
                });
                empty_set<T>([=](auto& is0) {
                        mem_insert()(is0, is0.end(), t);
                });
        });
        all_singleton_arrays<T>([](auto const& a1) {
                empty_set<T>([&](auto& is0) {
                        mem_insert()(is0, a1.begin(), a1.end());
                });
        });
        all_singleton_ilists<T>([](auto ilist1) {
                empty_set<T>([&](auto& is0) {
                        mem_insert()(is0, ilist1);
                });
        });

        full_set<T>([](auto& isN) {
                all_values<T>([&](auto const& k) {
                        mem_erase()(isN, k);
                });
        });

        // boost::container::flat_set<int>::erase invalidates iterators
        if constexpr (!std::is_same_v<T, boost::container::flat_set<int>>) {
                full_set<T>([](auto& isN) {
                        for (auto first = isN.begin(), last = isN.end(); first != last; /* expression inside loop */) {
                                mem_erase()(isN, first++);
                        }
                });
                full_set<T>([](auto& isN) {
                        mem_erase()(isN, isN.begin(), isN.end());
                });
        }

        all_cardinality_sets<T>(mem_clear());
        all_singleton_sets<T>(mem_clear());

        all_cardinality_sets<T>(op_equal_to());
        all_singleton_sets<T>(op_equal_to());

        all_cardinality_sets<T>(op_compare_three_way());
        all_singleton_sets<T>(op_compare_three_way());

        all_cardinality_sets<T>([](auto& is) {
                fn_iterator()(is);
        });
        all_cardinality_sets<T>([](auto const& is) {
                fn_iterator()(is);
        });

        all_cardinality_sets<T>(fn_size());
        all_cardinality_sets<T>(fn_ssize());
        all_cardinality_sets<T>(fn_empty());
}

BOOST_AUTO_TEST_SUITE_END()
