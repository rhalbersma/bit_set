//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <set/composable.hpp>           // includes
#include <set/exhaustive.hpp>           // all_doubleton_set_pairs
#include <set/flat_set.hpp>             // XSTD_TEST_HAS_FLAT_SET
#include <set/primitives.hpp>           // op_compare_three_way
#include <xstd/bits/finite_bit_set.hpp>             // finite_bit_set
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <set>                          // set
#include <tuple>                        // tuple

BOOST_AUTO_TEST_SUITE(Quartic)

using namespace xstd;

using Types = std::tuple
<       std::set<std::size_t>
#ifdef XSTD_TEST_HAS_FLAT_SET
,       std::flat_set<std::size_t>
#endif
,       finite_bit_set< 0, uint8_t>
,       finite_bit_set< 8, uint8_t>
,       finite_bit_set< 9, uint8_t>
,       finite_bit_set<17, uint8_t>
,       finite_bit_set<17, uint16_t>
,       finite_bit_set<17, uint32_t>
,       finite_bit_set<17, uint64_t>
#if defined(__GNUG__)
,       finite_bit_set<17, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(CompareThreeWay, T, Types)
{
        on4::all_doubleton_set_pairs<T>(op_compare_three_way());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Includes, T, Types)
{
        on4::all_doubleton_set_pairs<T>(composable::includes());
}

BOOST_AUTO_TEST_SUITE_END()
