//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <set/exhaustive.hpp>           // all_singleton_set_triples
#include <set/primitives.hpp>           // op_equal_to, op_less
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Cubic)

using namespace xstd;

using Key = int;
using Types = boost::mp11::mp_list
<       std::set<Key>
,       boost::container::flat_set<Key>
,       bit_set<Key,  0, uint8_t>
,       bit_set<Key,  8, uint8_t>
,       bit_set<Key,  9, uint8_t>
,       bit_set<Key, 17, uint8_t>
,       bit_set<Key, 17, uint16_t>
,       bit_set<Key, 17, uint32_t>
,       bit_set<Key, 17, uint64_t>
#if defined(__GNUG__)
,       bit_set<Key, 17, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Transitivity, T, Types)
{
        all_singleton_set_triples<T>(op_less());
}

BOOST_AUTO_TEST_SUITE_END()
