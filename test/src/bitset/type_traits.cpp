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

using namespace xstd;

using Types = boost::mp11::mp_list
<       std::bitset<  0>
,       std::bitset< 64>
,       std::bitset<128>
,       std::bitset<192>
,       std::bitset<256>
,       boost::dynamic_bitset<>
,       bitset<  0, uint8_t>
,       bitset<  8, uint8_t>
,       bitset< 16, uint8_t>
,       bitset< 24, uint8_t>
,       bitset<  0, uint16_t>
,       bitset< 16, uint16_t>
,       bitset< 32, uint16_t>
,       bitset< 48, uint16_t>
,       bitset<  0, uint32_t>
,       bitset< 32, uint32_t>
,       bitset< 64, uint32_t>
,       bitset< 96, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bitset<  0, uint64_t>
,       bitset< 64, uint64_t>
,       bitset<128, uint64_t>
,       bitset<192, uint64_t>
#endif
#if defined(__GNUG__)
,       bitset<  0, __uint128_t>
,       bitset<128, __uint128_t>
,       bitset<256, __uint128_t>
,       bitset<384, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Regular, T, Types)
{
        static_assert(std::regular<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Trivial, T, Types)
{
        if constexpr (!dynamic<T>) {
                static_assert(std::is_trivially_destructible_v<T>);
                static_assert(std::is_nothrow_default_constructible_v<T>);
                static_assert(std::is_trivially_copy_constructible_v<T>);
                static_assert(std::is_trivially_copy_assignable_v<T>);
                static_assert(std::is_trivially_move_constructible_v<T>);
                static_assert(std::is_trivially_move_assignable_v<T>);
        }
}

BOOST_AUTO_TEST_SUITE_END()
