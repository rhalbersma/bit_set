//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <xstd/bitset.hpp>              // bitset
#include <bitset/exhaustive.hpp>        // all_cardinality_sets, all_singleton_sets, all_valid, any_value, empty_set, full_set
#include <bitset/primitives.hpp>        // mem_set, mem_reset, mem_bit_not, mem_flip,
                                        // mem_at,
                                        // mem_count, mem_size, mem_test, mem_all, mem_any, mem_none,
                                        // op_iostream
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Linear)

using Types = boost::mp11::mp_list
<       boost::dynamic_bitset<>
,         std::bitset<  0>
,         std::bitset<  1>
,         std::bitset< 64>
,        xstd::bitset< 0, uint8_t>
,        xstd::bitset< 1, uint8_t>
,        xstd::bitset< 8, uint8_t>
,        xstd::bitset< 9, uint8_t>
,        xstd::bitset<16, uint8_t>
,        xstd::bitset<17, uint8_t>
,        xstd::bitset<24, uint8_t>
,        xstd::bitset<24, uint16_t>
,        xstd::bitset<24, uint32_t>
,        xstd::bitset<24, uint64_t>
#if defined(__GNUG__)
,        xstd::bitset<24, __uint128_t>
#endif
>;

using namespace xstd;

BOOST_AUTO_TEST_CASE_TEMPLATE(Operations, T, Types)
{
        on1::all_cardinality_sets<T>(mem_set());
        on1::all_cardinality_sets<T>(mem_set());
        on1::all_singleton_sets<T>(mem_set());
        on1::any_value<T>([](auto pos) {
                on0::empty_set<T>([&](auto& bs0) {
                        mem_set()(bs0, pos);
                });
                on0::empty_set<T>([&](auto& bs0) {
                        mem_set()(bs0, pos, true);
                });
                on0::full_set<T>([&](auto& bsN) {
                        mem_set()(bsN, pos, false);
                });
        });

        on1::all_cardinality_sets<T>(mem_reset());
        on1::all_singleton_sets<T>(mem_reset());
        on1::any_value<T>([](auto pos) {
                on0::full_set<T>([&](auto& bsN) {
                        mem_reset()(bsN, pos);
                });
        });

        on1::all_cardinality_sets<T>(mem_bit_not());
        on1::all_singleton_sets<T>(mem_bit_not());

        on1::all_cardinality_sets<T>(mem_flip());
        on1::all_singleton_sets<T>(mem_flip());
        on1::any_value<T>([](auto pos) {
                on0::empty_set<T>([&](auto& bs0) {
                        mem_flip()(bs0, pos);
                });
                on0::full_set<T>([&](auto& bsN) {
                        mem_flip()(bsN, pos);
                });
       });
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ElementAccess, T, Types)
{
        on1::all_valid<T>([](auto pos) {
                on0::empty_set<T>([&](auto const& bs0) {
                        mem_at()(bs0, pos);
                });
                // empty_set<T>([&](auto& bs0) {
                //         mem_at()(bs0, pos);
                // });
                // empty_set<T>([&](auto& bs0) {
                //         mem_at()(bs0, pos, false);
                // });
                // empty_set<T>([&](auto& bs0) {
                //         mem_at()(bs0, pos, true);
                // });
                on0::full_set<T>([&](auto const& bsN) {
                        mem_at()(bsN, pos);
                });
                // full_set<T>([&](auto& bsN) {
                //         mem_at()(bsN, pos);
                // });
                // full_set<T>([&](auto& bsN) {
                //         mem_at()(bsN, pos, false);
                // });
                // full_set<T>([&](auto& bsN) {
                //         mem_at()(bsN, pos, true);
                // });
        });
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Observers, T, Types)
{
        on1::all_cardinality_sets<T>(mem_count());

        on1::all_cardinality_sets<T>(mem_size());
        on1::all_singleton_sets<T>(mem_size());

        on1::any_value<T>([](auto pos) {
                on0::empty_set<T>([&](auto const& bs0) {
                        mem_test()(bs0, pos);
                });
                on0::full_set<T>([&](auto const& bsN) {
                        mem_test()(bsN, pos);
                });
        });

        on1::all_cardinality_sets<T>(mem_all());
        on1::all_cardinality_sets<T>(mem_any());
        on1::all_cardinality_sets<T>(mem_none());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Operators, T, Types)
{
        on1::all_cardinality_sets<T>(op_iostream());
        on1::all_singleton_sets<T>(op_iostream());
}

BOOST_AUTO_TEST_SUITE_END()
