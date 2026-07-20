//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/xstd/bitset.hpp>          // bitset
#include <bitset/exhaustive.hpp>        // all_singleton_sets, all_doubleton_sets, all_triplet_sets, empty_set, full_set
#include <bitset/primitives.hpp>        // mem_compare_three_way
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Cubic)

using namespace xstd;

using Types = boost::mp11::mp_list
<       boost::dynamic_bitset<>
,         std::bitset< 0>
,         std::bitset<17>
,        xstd::bitset< 0, uint8_t>
,        xstd::bitset< 8, uint8_t>
,        xstd::bitset< 9, uint8_t>
,        xstd::bitset<17, uint8_t>
,        xstd::bitset<17, uint16_t>
,        xstd::bitset<17, uint32_t>
,        xstd::bitset<17, uint64_t>
#if defined(__GNUG__)
,        xstd::bitset<17, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Observers, T, Types)
{
        // empty/full set vs. every triplet, at matching N - see o1.cpp's
        // identical singleton case for why N is pinned explicitly.
        on3::all_triplet_sets<T>([](auto const& bs3) {
                on0::empty_set<T, limit_v<T, L3>>([&](auto const& bs0) {
                        mem_compare_three_way()(bs0, bs3);
                });
                on0::full_set<T, limit_v<T, L3>>([&](auto const& bsN) {
                        mem_compare_three_way()(bsN, bs3);
                });
        });

        // every singleton vs. every doubleton, at matching N.
        on2::all_doubleton_sets<T, limit_v<T, L3>>([](auto const& bs2) {
                on1::all_singleton_sets<T, limit_v<T, L3>>([&](auto const& bs1) {
                        mem_compare_three_way()(bs1, bs2);
                });
        });
}

BOOST_AUTO_TEST_SUITE_END()
