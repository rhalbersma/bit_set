//          Copyright Rein Halbersma 2014-2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <adaptor/dynamic_bitset.hpp>   // dynamic_bitset
#include <adaptor/std_bitset.hpp>       // bitset
#include <adaptor/xstd_bitset.hpp>      // bitset
#include <bitset/primitives.hpp>        // constructor
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Constant)

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
,       bitset<  0, uint8_t>
,       bitset<  1, uint8_t>
,       bitset<  7, uint8_t>
,       bitset<  8, uint8_t>
,       bitset<  9, uint8_t>
,       bitset< 15, uint8_t>
,       bitset< 16, uint8_t>
,       bitset< 17, uint8_t>
,       bitset< 24, uint8_t>
,       bitset<  0, uint16_t>
,       bitset<  1, uint16_t>
,       bitset< 15, uint16_t>
,       bitset< 16, uint16_t>
,       bitset< 17, uint16_t>
,       bitset< 31, uint16_t>
,       bitset< 32, uint16_t>
,       bitset< 33, uint16_t>
,       bitset< 48, uint16_t>
,       bitset<  0, uint32_t>
,       bitset<  1, uint32_t>
,       bitset< 31, uint32_t>
,       bitset< 32, uint32_t>
,       bitset< 33, uint32_t>
,       bitset< 63, uint32_t>
,       bitset< 64, uint32_t>
,       bitset< 65, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bitset<  0, uint64_t>
,       bitset<  1, uint64_t>
,       bitset< 63, uint64_t>
,       bitset< 64, uint64_t>
,       bitset< 65, uint64_t>
#endif
#if defined(__GNUG__)
,       bitset<  0, __uint128_t>
,       bitset<  1, __uint128_t>
,       bitset<127, __uint128_t>
,       bitset<128, __uint128_t>
,       bitset<129, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(BitSet, T, bitset_types)
{
        constructor<T>()();
}

BOOST_AUTO_TEST_SUITE_END()
