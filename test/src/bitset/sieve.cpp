//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <opt/bitset/sieve.hpp>         // filter_twins, sift_primes0, sift_primes1
#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/std/ranges.hpp>           // as_set
#include <xstd/bitset.hpp>              // bitset
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <fmt/format.h>                 // format
#include <fmt/ranges.h>

BOOST_AUTO_TEST_SUITE(Sieve)

inline constexpr auto N = 100uz;

using Types = boost::mp11::mp_list
<       boost::dynamic_bitset<>
,         std::bitset<N>
,        xstd::bitset<N>
,        xstd::bit_set<N>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Format, T, Types)
{
        auto const primes0 = xstd::sift_primes0<T>(N);
        BOOST_CHECK_EQUAL(
                fmt::format("{}", primes0 | xstd::views::as_set),
                "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}")
        ;

        auto const primes1 = xstd::sift_primes1<T>(N);
        BOOST_CHECK_EQUAL(
                fmt::format("{}", primes1 | xstd::views::as_set),
                "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}")
        ;

        auto const twins = xstd::filter_twins(primes1);
        BOOST_CHECK_EQUAL(
                fmt::format("{}", twins | xstd::views::as_set),
                "{3, 5, 7, 11, 13, 17, 19, 29, 31, 41, 43, 59, 61, 71, 73}"
        );
}

BOOST_AUTO_TEST_SUITE_END()
