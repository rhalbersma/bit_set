//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK_EQUAL_COLLECTIONS
#include <algorithm>                    // copy
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <iterator>                     // inserter
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Implicit)

using namespace xstd;

using int_set_types = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       bit_set< 32, uint16_t>
,       bit_set< 33, uint16_t>
,       bit_set< 48, uint16_t>
,       bit_set< 32, uint32_t>
,       bit_set< 33, uint32_t>
,       bit_set< 64, uint32_t>
,       bit_set< 65, uint32_t>
,       bit_set< 96, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bit_set< 64, uint64_t>
,       bit_set< 65, uint64_t>
,       bit_set<128, uint64_t>
,       bit_set<129, uint64_t>
,       bit_set<192, uint64_t>
#endif
#if defined(__GNUG__)
,       bit_set<128, __uint128_t>
,       bit_set<129, __uint128_t>
,       bit_set<256, __uint128_t>
,       bit_set<257, __uint128_t>
,       bit_set<384, __uint128_t>
#endif
>;

class int_
{
        int m_value;
public:
        XSTD_PP_EXPLICIT_FALSE int_(int v) noexcept : m_value{v} {}
        XSTD_PP_EXPLICIT_FALSE operator int() const noexcept { return m_value; }
};

BOOST_AUTO_TEST_CASE_TEMPLATE(IntConstructible, T, int_set_types)
{
        auto src = T { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };
        std::set<int_> dst;
        std::copy(src.begin(), src.end(), std::inserter(dst, dst.end()));
        BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(), dst.begin(), dst.end());
}

BOOST_AUTO_TEST_SUITE_END()
