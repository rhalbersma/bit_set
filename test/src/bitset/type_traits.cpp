//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <xstd/bitset.hpp>              // bitset
#include <xstd/bit_set.hpp>
#include <concepts.hpp>                 // dynamic
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <concepts>                     // regular
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <type_traits>                  // is_trivially

BOOST_AUTO_TEST_SUITE(TypeTraits)

using Types = boost::mp11::mp_list
<       std::bitset<  0>
,       std::bitset< 64>
,       std::bitset<128>
,       std::bitset<192>
,       std::bitset<256>
,       boost::dynamic_bitset<>
,       xstd::bitset<  0, uint8_t>
,       xstd::bitset<  8, uint8_t>
,       xstd::bitset< 16, uint8_t>
,       xstd::bitset< 24, uint8_t>
,       xstd::bitset<  0, uint16_t>
,       xstd::bitset< 16, uint16_t>
,       xstd::bitset< 32, uint16_t>
,       xstd::bitset< 48, uint16_t>
,       xstd::bitset<  0, uint32_t>
,       xstd::bitset< 32, uint32_t>
,       xstd::bitset< 64, uint32_t>
,       xstd::bitset< 96, uint32_t>
,       xstd::bitset<  0, uint64_t>
,       xstd::bitset< 64, uint64_t>
,       xstd::bitset<128, uint64_t>
,       xstd::bitset<192, uint64_t>
#if defined(__GNUG__)
,       xstd::bitset<  0, __uint128_t>
,       xstd::bitset<128, __uint128_t>
,       xstd::bitset<256, __uint128_t>
,       xstd::bitset<384, __uint128_t>
#endif
>;

using namespace xstd;

BOOST_AUTO_TEST_CASE_TEMPLATE(Regular, T, Types)
{
        static_assert(std::regular<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Trivial, T, Types)
{
        if constexpr (not dynamic<T>) {
                static_assert(std::is_trivially_destructible_v<T>);
                static_assert(std::is_nothrow_default_constructible_v<T>);
                static_assert(std::is_trivially_copy_constructible_v<T>);
                static_assert(std::is_trivially_copy_assignable_v<T>);
                static_assert(std::is_trivially_move_constructible_v<T>);
                static_assert(std::is_trivially_move_assignable_v<T>);
        }
}

BOOST_AUTO_TEST_SUITE_END()
