//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <aux/set/sieve.hpp>            // filter_twins, sift_primes
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <fmt/format.h>                 // format
#include <fmt/ranges.h>
#include <cstddef>                      // size_t
#include <locale>                       // locale
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Sieve)

inline constexpr auto N = 100uz;

using set_types = boost::mpl::vector
<       std::set<std::size_t>
,       boost::container::flat_set<std::size_t>
,       xstd::bit_set<N>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Format, T, set_types)
{
        std::locale::global(std::locale("en_US.UTF-8"));

        auto const primes = xstd::sift_primes<T>(N);
        BOOST_CHECK_EQUAL(
                fmt::format("{}", primes),
                "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}"
        );

        auto const twins = xstd::filter_twins(primes);
        BOOST_CHECK_EQUAL(
                fmt::format("{}", twins),
                "{3, 5, 7, 11, 13, 17, 19, 29, 31, 41, 43, 59, 61, 71, 73}"
        );
}

BOOST_AUTO_TEST_SUITE_END()
