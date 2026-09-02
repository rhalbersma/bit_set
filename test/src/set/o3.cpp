//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/test/set/exhaustive.hpp>           // all_singleton_set_triples
#include <xstd/test/flat_set.hpp>             // XSTD_TEST_HAS_FLAT_SET
#include <xstd/test/set/primitives.hpp>           // op_equal_to, op_less
#include <xstd/bits/bit_finite_set.hpp>             // bit_finite_set
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <set>                          // set
#include <tuple>                        // tuple

BOOST_AUTO_TEST_SUITE(Cubic)

using namespace xstd;
using namespace xstd::test;
using namespace xstd::test::set;

using Types = std::tuple
<       std::set<std::size_t>
#ifdef XSTD_TEST_HAS_FLAT_SET
,       std::flat_set<std::size_t>
#endif
,       bit_finite_set< 0, uint8_t>
,       bit_finite_set< 8, uint8_t>
,       bit_finite_set< 9, uint8_t>
,       bit_finite_set<17, uint8_t>
,       bit_finite_set<17, uint16_t>
,       bit_finite_set<17, uint32_t>
,       bit_finite_set<17, uint64_t>
#if defined(__GNUG__)
,       bit_finite_set<17, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Transitivity, T, Types)
{
        on3::all_singleton_set_triples<T>(op_less());
}

BOOST_AUTO_TEST_SUITE_END()
