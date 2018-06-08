//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>                               // bitset
#include <xstd/int_set.hpp>                     // int_set
#include <legacy.hpp>                           // bitset, int_set
#include <exhaustive.hpp>                       // all_singleton_set_triples
#include <primitive.hpp>                        // op_equal_to, op_less, fn_is_subset_of,
                                                // op_bitand, op_bitor, op_xor
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Cubic)

using namespace xstd;

using SetTypes = boost::mpl::vector
<       std::bitset< 0>
,       std::bitset<32>
,       std::bitset<64>
,       int_set< 0, uint32_t>
,       int_set<32, uint32_t>
,       int_set<64, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Transitivity, T, SetTypes)
{
        all_singleton_set_triples<T>(op_equal_to{});
        all_singleton_set_triples<T>(op_less{});
        all_singleton_set_triples<T>(fn_is_subset_of{});
}

BOOST_AUTO_TEST_CASE_TEMPLATE(AssociativityAndDistributivity, T, SetTypes)
{
        all_singleton_set_triples<T>(op_bitand{});
        all_singleton_set_triples<T>(op_bitor{});
        all_singleton_set_triples<T>(op_xor{});
}

BOOST_AUTO_TEST_SUITE_END()
