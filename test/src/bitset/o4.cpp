//          Copyright Rein Halbersma 2014-2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <bitset/compatible/bit_set.hpp>        // bit_set
#include <bitset/compatible/bitset.hpp>         // bitset
#include <bitset/compatible/dynamic_bitset.hpp> // dynamic_bitset
#include <bitset/exhaustive.hpp>                // all_doubleton_set_pairs
#include <bitset/primitives.hpp>                // mem_is_subset_of, mem_is_proper_subset_of
#include <xstd/bit_set.hpp>                     // bit_set
#include <boost/dynamic_bitset.hpp>             // dynamic_bitset
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                              // uint8_t, uint16_t, uint32_t
#include <bitset>                               // bitset

BOOST_AUTO_TEST_SUITE(Quartic)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset< 0>
,       std::bitset< 1>
,       std::bitset<31>
,       std::bitset<32>
,       boost::dynamic_bitset<>
,       bit_set< 0, uint8_t>
,       bit_set< 1, uint8_t>
,       bit_set< 7, uint8_t>
,       bit_set< 8, uint8_t>
,       bit_set< 9, uint8_t>
,       bit_set<15, uint8_t>
,       bit_set<16, uint8_t>
,       bit_set<17, uint8_t>
,       bit_set<24, uint8_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IsSubsetOf, T, bitset_types)
{
        all_doubleton_set_pairs<T>(mem_is_subset_of());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsProperSubsetOf, T, bitset_types)
{
        all_doubleton_set_pairs<T>(mem_is_proper_subset_of());
}

BOOST_AUTO_TEST_SUITE_END()
