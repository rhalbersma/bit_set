//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>                     // int_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK_EQUAL_COLLECTIONS
#include <algorithm>                            // copy
#include <iterator>                             // inserter
#include <set>                                  // set

BOOST_AUTO_TEST_SUITE(Implicit)

using IntSetTypes = boost::mpl::vector
<       std::set<int>
,       xstd::int_set<32>
>;

class Int
{
        int m_value;
public:
        /* explicit(false) */ Int(int v) noexcept : m_value{v} {}
        /* explicit(false) */ operator int() const noexcept { return m_value; }
};

BOOST_AUTO_TEST_CASE_TEMPLATE(IntConstructible, T, IntSetTypes)
{
        auto src = T { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };
        std::set<Int> dst;
        std::copy(src.begin(), src.end(), std::inserter(dst, dst.end()));
        BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(), dst.begin(), dst.end());
}

BOOST_AUTO_TEST_SUITE_END()
