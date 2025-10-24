//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>             // bit_set
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <concepts>                     // regular, totally_ordered
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <flat_set>                     // flat_set
#include <ranges>                       // bidirectional_range
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(TypeTraits)

using namespace xstd;

using Types = boost::mp11::mp_list
<       std::set<std::size_t>
,       std::flat_set<std::size_t>
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
,       bit_set<  0, uint64_t>
,       bit_set< 64, uint64_t>
,       bit_set<128, uint64_t>
,       bit_set<192, uint64_t>
#if defined(__GNUG__)
,       bit_set<  0, __uint128_t>
,       bit_set<128, __uint128_t>
,       bit_set<256, __uint128_t>
,       bit_set<384, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Regular, T, Types)
{
        static_assert(std::regular<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TotallyOrdered, T, Types)
{
        static_assert(std::totally_ordered<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BidirectionalRange, T, Types)
{
        static_assert(std::ranges::bidirectional_range<T>);
}

BOOST_AUTO_TEST_SUITE_END()
