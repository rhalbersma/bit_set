//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>                     // int_set
#include <set/exhaustive.hpp>                   // all_singleton_set_triples
#include <set/primitives.hpp>                   // op_equal_to, op_less
#include <boost/container/flat_set.hpp>         // flat_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <set>                                  // set

BOOST_AUTO_TEST_SUITE(Cubic)

using namespace xstd;

using int_set_types = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       int_set< 0, uint32_t>
,       int_set<32, uint32_t>
,       int_set<64, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Transitivity, T, int_set_types)
{
        all_singleton_set_triples<T>(op_equal_to{});
        all_singleton_set_triples<T>(op_less{});
}

BOOST_AUTO_TEST_SUITE_END()
