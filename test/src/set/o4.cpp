//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/container/flat_set.hpp>     // flat_set
#include <set/composable.hpp>                   // includes
#include <set/exhaustive.hpp>                   // all_doubleton_set_pairs
#include <set/primitives.hpp>                   // op_compare_three_way
#include <xstd/bit_set.hpp>                     // bit_set
#include <boost/mp11/list.hpp>                  // mp_list
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstddef>                              // size_t
#include <cstdint>                              // uint8_t, uint16_t, uint32_t, uint64_t
#include <set>                                  // set

BOOST_AUTO_TEST_SUITE(Quartic)

using namespace xstd;

using Types = boost::mp11::mp_list
<       std::set<std::size_t>
,       boost::container::flat_set<std::size_t>
,       bit_set< 0, uint8_t>
,       bit_set< 8, uint8_t>
,       bit_set< 9, uint8_t>
,       bit_set<17, uint8_t>
,       bit_set<17, uint16_t>
,       bit_set<17, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bit_set<17, uint64_t>
#endif
#if defined(__GNUG__)
,       bit_set<17, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(CompareThreeWay, T, Types)
{
        all_doubleton_set_pairs<T>(op_compare_three_way());
        all_doubleton_set_pairs<T>(composable::includes());
}

BOOST_AUTO_TEST_SUITE_END()
