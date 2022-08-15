//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset/compatible/bit_set.hpp>        // bit_set
#include <bitset/compatible/bitset.hpp>         // bitset
#include <bitset/compatible/dynamic_bitset.hpp> // dynamic_bitset
#include <bitset/exhaustive.hpp>                // all_singleton_set_triples
#include <bitset/primitives.hpp>                // mem_is_subset_of, op_bitand, op_bitor, op_xor
#include <xstd/bit_set.hpp>                     // bit_set
#include <boost/dynamic_bitset.hpp>             // dynamic_bitset
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <bitset>                               // bitset
#include <cstdint>                              // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Cubic)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset< 0>
,       std::bitset<17>
,       boost::dynamic_bitset<>
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

BOOST_AUTO_TEST_CASE_TEMPLATE(Transitivity, T, bitset_types)
{
        all_singleton_set_triples<T>(mem_is_subset_of());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(AssociativityAndDistributivity, T, bitset_types)
{
        all_singleton_set_triples<T>(op_bitand());
        all_singleton_set_triples<T>(op_bitor());
        all_singleton_set_triples<T>(op_xor());
}

BOOST_AUTO_TEST_SUITE_END()
