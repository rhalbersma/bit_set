//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <set/composable.hpp>           // includes, set_difference, set_intersection, set_symmetric_difference, set_union
#include <set/exhaustive.hpp>           // empty_set_pair
#include <set/primitives.hpp>           // constructor mem_swap,fn_swap, op_equal, op_not_equal_to,
                                        // op_compare_three_way op_less, op_greater, op_less_equal, op_greater_equal,
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <flat_set>                     // flat_set
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Constant)

using namespace xstd;

using Types = boost::mp11::mp_list
<       std::set<std::size_t>
,       std::flat_set<std::size_t>
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
,       bit_set<  0, uint64_t>
,       bit_set<  1, uint64_t>
,       bit_set< 63, uint64_t>
,       bit_set< 64, uint64_t>
,       bit_set< 65, uint64_t>
#if defined(__GNUG__)
,       bit_set<  0, __uint128_t>
,       bit_set<  1, __uint128_t>
,       bit_set<127, __uint128_t>
,       bit_set<128, __uint128_t>
,       bit_set<129, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IntSet, T, Types)
{
        [[maybe_unused]] auto _ = nested_types<T>();
        constructor<T>()();

        on0::empty_set_pair<T>(mem_swap());
        on0::empty_set_pair<T>(fn_swap());

        on0::empty_set_pair<T>(op_equal_to());
        on0::empty_set_pair<T>(op_not_equal_to());

        on0::empty_set_pair<T>(op_compare_three_way());
        on0::empty_set_pair<T>(op_less());
        on0::empty_set_pair<T>(op_greater());
        on0::empty_set_pair<T>(op_less_equal());
        on0::empty_set_pair<T>(op_greater_equal());

        on0::empty_set_pair<T>(composable::includes());
        on0::empty_set_pair<T>(composable::set_union());
        on0::empty_set_pair<T>(composable::set_intersection());
        on0::empty_set_pair<T>(composable::set_difference());
        on0::empty_set_pair<T>(composable::set_symmetric_difference());
}

BOOST_AUTO_TEST_SUITE_END()
