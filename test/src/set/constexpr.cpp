//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <xstd/bit_set.hpp>             // bit_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK_EQUAL_COLLECTIONS
#include <compare>                      // strong_ordering

BOOST_AUTO_TEST_SUITE(Constexpr)

using namespace xstd;

using int_set_types = boost::mpl::vector
<       bit_set<  0, uint8_t>
,       bit_set<  1, uint8_t>
,       bit_set<  7, uint8_t>
,       bit_set<  8, uint8_t>
,       bit_set<  9, uint8_t>
,       bit_set< 15, uint8_t>
,       bit_set< 16, uint8_t>
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=94933
// ,       bit_set< 17, uint8_t> 
// ,       bit_set< 24, uint8_t>
,       bit_set<  0, uint16_t>
,       bit_set<  1, uint16_t>
,       bit_set< 15, uint16_t>
,       bit_set< 16, uint16_t>
,       bit_set< 17, uint16_t>
,       bit_set< 31, uint16_t>
,       bit_set< 32, uint16_t>
,       bit_set< 33, uint16_t>
,       bit_set< 48, uint16_t>
,       bit_set<  0, uint32_t>
,       bit_set<  1, uint32_t>
,       bit_set< 31, uint32_t>
,       bit_set< 32, uint32_t>
,       bit_set< 33, uint32_t>
,       bit_set< 63, uint32_t>
,       bit_set< 64, uint32_t>
,       bit_set< 65, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bit_set<  0, uint64_t>
,       bit_set<  1, uint64_t>
,       bit_set< 63, uint64_t>
,       bit_set< 64, uint64_t>
,       bit_set< 65, uint64_t>
#endif
#if defined(__GNUG__)
,       bit_set<  0, __uint128_t>
,       bit_set<  1, __uint128_t>
,       bit_set<127, __uint128_t>
,       bit_set<128, __uint128_t>
,       bit_set<129, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Empty, T, int_set_types)
{
        constexpr auto b = T();
        static_assert(b == T().fill().clear());
        static_assert(b.empty());
        static_assert(b.size() == 0);
        static_assert(b.begin() == b.end());
        static_assert(b == b);
        static_assert((b <=> b) == std::strong_ordering::equal);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Full, T, int_set_types)
{
        constexpr auto b = ~T();
        static_assert(b == T().fill());
        static_assert(b.full());
        static_assert(b.size() == b.max_size());
        static_assert(b.empty() || b.front() == *b.cbegin());
        static_assert(b.empty() || b.back() == *b.crbegin());
        static_assert(b == b);
        static_assert((b <=> b) == std::strong_ordering::equal);
}

BOOST_AUTO_TEST_SUITE_END()
