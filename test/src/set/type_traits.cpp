//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>                     // int_set
#include <boost/container/flat_set.hpp>         // flat_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END
#include <set>                                  // set
#include <type_traits>                          // is_nothrow_default_constructible_v, is_same_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

using IntSetTypes = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       xstd::int_set<  0>
,       xstd::int_set< 32>
,       xstd::int_set< 64>
,       xstd::int_set<128>
,       xstd::int_set<256>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IsNothrowDefaultConstructible, T, IntSetTypes)
{
        static_assert(std::is_nothrow_default_constructible_v<T>);
}

BOOST_AUTO_TEST_SUITE_END()
