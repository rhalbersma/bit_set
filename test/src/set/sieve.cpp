//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <opt/set/sieve.hpp>            // filter_twins, sift_primes0, sift_primes1
#include <xstd/test/flat_set.hpp>             // XSTD_TEST_HAS_FLAT_SET
#include <xstd/bits/bit_finite_set.hpp>             // bit_finite_set
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <fmt/format.h>                 // format
#include <fmt/ranges.h>
#include <cstddef>                      // size_t
#include <set>                          // set
#include <tuple>                        // tuple

BOOST_AUTO_TEST_SUITE(Sieve)

inline constexpr auto N = 100uz;

using Types = std::tuple
<       std::set<std::size_t>
#ifdef XSTD_TEST_HAS_FLAT_SET
,       std::flat_set<std::size_t>
#endif
,       xstd::bit_finite_set<N>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Format, T, Types)
{
        auto const primes0 = xstd::sift_primes0<T>(N);
        BOOST_CHECK_EQUAL(
                fmt::format("{}", primes0),
                "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}"
        );

        auto const primes1 = xstd::sift_primes1<T>(N);
        BOOST_CHECK_EQUAL(
                fmt::format("{}", primes1),
                "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}"
        );

        auto const twins = xstd::filter_twins(primes1);
        BOOST_CHECK_EQUAL(
                fmt::format("{}", twins),
                "{3, 5, 7, 11, 13, 17, 19, 29, 31, 41, 43, 59, 61, 71, 73}"
        );
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Degenerate, T, Types)
{
        // Below three candidates sift_primes1 runs its loop to exhaustion
        // rather than breaking on the first p whose square is out of range,
        // and filter_twins returns before it has a triple to compare.
        auto const none = xstd::sift_primes1<T>(2);
        BOOST_CHECK(none.empty());
        BOOST_CHECK(xstd::filter_twins(none).empty());

        auto const one = xstd::sift_primes1<T>(3);
        BOOST_CHECK_EQUAL(fmt::format("{}", one), "{2}");
        BOOST_CHECK(xstd::filter_twins(one).empty());
}

BOOST_AUTO_TEST_SUITE_END()
