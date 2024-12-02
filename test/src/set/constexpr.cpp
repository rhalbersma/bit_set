//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>             // bit_set
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK_EQUAL_COLLECTIONS
#include <compare>                      // strong_ordering

BOOST_AUTO_TEST_SUITE(Constexpr)

using namespace xstd;

using Key = int;
using Types = boost::mp11::mp_list
<       bit_set<Key,    0, uint8_t>
,       bit_set<Key,    1, uint8_t>
,       bit_set<Key,    7, uint8_t>
,       bit_set<Key,    8, uint8_t>
,       bit_set<Key,    9, uint8_t>
,       bit_set<Key,   15, uint8_t>
,       bit_set<Key,   16, uint8_t>
,       bit_set<Key,   17, uint8_t>
,       bit_set<Key,   24, uint8_t>
,       bit_set<Key,    0, uint16_t>
,       bit_set<Key,    1, uint16_t>
,       bit_set<Key,   15, uint16_t>
,       bit_set<Key,   16, uint16_t>
,       bit_set<Key,   17, uint16_t>
,       bit_set<Key,   31, uint16_t>
,       bit_set<Key,   32, uint16_t>
,       bit_set<Key,   33, uint16_t>
,       bit_set<Key,   48, uint16_t>
,       bit_set<Key,    0, uint32_t>
,       bit_set<Key,    1, uint32_t>
,       bit_set<Key,   31, uint32_t>
,       bit_set<Key,   32, uint32_t>
,       bit_set<Key,   33, uint32_t>
,       bit_set<Key,   63, uint32_t>
,       bit_set<Key,   64, uint32_t>
,       bit_set<Key,   65, uint32_t>
,       bit_set<Key,    0, uint64_t>
,       bit_set<Key,    1, uint64_t>
,       bit_set<Key,   63, uint64_t>
,       bit_set<Key,   64, uint64_t>
,       bit_set<Key,   65, uint64_t>
#if defined(__GNUG__)
,       bit_set<Key,    0, __uint128_t>
,       bit_set<Key,    1, __uint128_t>
,       bit_set<Key,  127, __uint128_t>
,       bit_set<Key,  128, __uint128_t>
,       bit_set<Key,  129, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Empty, T, Types)
{
        constexpr auto b = T();
        static_assert(b.empty());
        static_assert(b.size() == 0);
        static_assert(b.begin() == b.end());
        static_assert(b == b);
        static_assert((b <=> b) == std::strong_ordering::equal);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Full, T, Types)
{
        constexpr auto b = ~T();
        static_assert(b.full());
        static_assert(b.size() == b.max_size());
        static_assert(b.empty() || b.front() == *b.cbegin());
        static_assert(b.empty() || b.back()  == *b.crbegin());
        static_assert(b == b);
        static_assert((b <=> b) == std::strong_ordering::equal);
}

BOOST_AUTO_TEST_SUITE_END()
