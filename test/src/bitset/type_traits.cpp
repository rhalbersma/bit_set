//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <concepts.hpp>                 // resizeable
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <bitset>                       // bitset
#include <concepts>                     // regular, same_as
#include <type_traits>                  // is_nothrow_default_constructible_v, is_standard_layout, is_trivially_copyable 

BOOST_AUTO_TEST_SUITE(TypeTraits)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset< 64>
,       std::bitset<128>
,       std::bitset<192>
,       std::bitset<256>
,       boost::dynamic_bitset<>
,       bit_set<  0, uint8_t>
,       bit_set<  8, uint8_t>
,       bit_set< 16, uint8_t>
,       bit_set< 24, uint8_t>
,       bit_set<  0, uint16_t>
,       bit_set< 16, uint16_t>
,       bit_set< 32, uint16_t>
,       bit_set< 48, uint16_t>
,       bit_set<  0, uint32_t>
,       bit_set< 32, uint32_t>
,       bit_set< 64, uint32_t>
,       bit_set< 96, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bit_set<  0, uint64_t>
,       bit_set< 64, uint64_t>
,       bit_set<128, uint64_t>
,       bit_set<192, uint64_t>
#endif
#if defined(__GNUG__)
,       bit_set<  0, __uint128_t>
,       bit_set<128, __uint128_t>
,       bit_set<256, __uint128_t>
,       bit_set<384, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IsRegular, T, bitset_types)
{
        static_assert(std::regular<T>);
}

BOOST_AUTO_TEST_SUITE_END()
