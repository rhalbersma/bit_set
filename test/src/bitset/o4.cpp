//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/xstd/bitset.hpp>          // bitset
#include <bitset/exhaustive.hpp>        // all_doubleton_set_pairs
#include <bitset/primitives.hpp>        // mem_compare_three_way, mem_is_subset_of, mem_is_proper_subset_of
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Quartic)

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
        on4::all_doubleton_set_pairs<T>(mem_compare_three_way());
        on4::all_doubleton_set_pairs<T>(mem_is_subset_of());
        on4::all_doubleton_set_pairs<T>(mem_is_proper_subset_of());
}

BOOST_AUTO_TEST_SUITE_END()
