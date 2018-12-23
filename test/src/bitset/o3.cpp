//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <bitset/compatible/bitset.hpp>         // bitset
#include <bitset/compatible/dynamic_bitset.hpp> // dynamic_bitset
#include <bitset/compatible/int_set.hpp>        // int_set
#include <bitset/exhaustive.hpp>                // all_singleton_set_triples
#include <bitset/primitives.hpp>                // fn_is_subset_of, op_bitand, op_bitor, op_xor
#include <xstd/int_set.hpp>                     // int_set
#include <boost/dynamic_bitset.hpp>             // dynamic_bitset
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <bitset>                               // bitset
#include <cstdint>                              // uint8_t, uint16_t, uint32_t

BOOST_AUTO_TEST_SUITE(Cubic)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset<  1>
,       std::bitset< 31>
,       std::bitset< 32>
,       std::bitset< 33>
,       std::bitset< 63>
,       std::bitset< 64>
,       std::bitset< 65>
,       boost::dynamic_bitset<>
,       int_set<  0, uint8_t>
,       int_set<  1, uint8_t>
,       int_set<  7, uint8_t>
,       int_set<  8, uint8_t>
,       int_set<  9, uint8_t>
,       int_set< 15, uint8_t>
,       int_set< 16, uint8_t>
,       int_set< 17, uint8_t>
,       int_set<  0, uint16_t>
,       int_set<  1, uint16_t>
,       int_set< 15, uint16_t>
,       int_set< 16, uint16_t>
,       int_set< 17, uint16_t>
,       int_set< 31, uint16_t>
,       int_set< 32, uint16_t>
,       int_set< 33, uint16_t>
,       int_set<  0, uint32_t>
,       int_set<  1, uint32_t>
,       int_set< 31, uint32_t>
,       int_set< 32, uint32_t>
,       int_set< 33, uint32_t>
,       int_set< 63, uint32_t>
,       int_set< 64, uint32_t>
,       int_set< 65, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Transitivity, T, bitset_types)
{
        all_singleton_set_triples<T>(fn_is_subset_of{});
}

BOOST_AUTO_TEST_CASE_TEMPLATE(AssociativityAndDistributivity, T, bitset_types)
{
        all_singleton_set_triples<T>(op_bitand{});
        all_singleton_set_triples<T>(op_bitor{});
        all_singleton_set_triples<T>(op_xor{});
}

BOOST_AUTO_TEST_SUITE_END()
