//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <adaptor/dynamic_bitset.hpp>   // dynamic_bitset
#include <adaptor/std_bitset.hpp>       // bitset
#include <adaptor/xstd_bitset.hpp>      // bitset
#include <bitset/exhaustive.hpp>        // all_doubleton_set_pairs
#include <bitset/primitives.hpp>        // mem_is_subset_of, mem_is_proper_subset_of
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Quartic)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset< 0>
,       std::bitset<17>
,       boost::dynamic_bitset<>
,       bitset< 0, uint8_t>
,       bitset< 8, uint8_t>
,       bitset< 9, uint8_t>
,       bitset<17, uint8_t>
,       bitset<17, uint16_t>
,       bitset<17, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bitset<17, uint64_t>
#endif
#if defined(__GNUG__)
,       bitset<17, __uint128_t>
#endif
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
