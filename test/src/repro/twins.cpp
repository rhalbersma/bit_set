//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// TEMPORARY, part of the MinGW Debug segfault investigation.
//
// test.set.sieve dies in xstd::filter_twins (include/opt/set/sieve.hpp:96)
// with X = std::set<std::size_t>, on the first begin() of the view pipeline,
// about 9 KB into a stack that has megabytes left. This file cuts that one
// call into its independent parts, so a single CI run says which part fails
// instead of leaving the whole pipeline as one suspect.
//
// Every case here is a subset of what test.set.sieve already does, with
// Boost.Test's own machinery, fmt, and the other twelve types removed. It
// comes out once the crash is diagnosed.

#include <opt/set/sieve.hpp>            // filter_twins, sift_primes1
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <cstddef>                      // size_t
#include <ranges>                       // adjacent, elements, filter
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Repro)

using S = std::set<std::size_t>;

inline constexpr auto N = 100UZ;

// The 25 primes below 100, as a literal, so a case can use a known-good set
// without going through sift_primes1 first.
static auto known_primes()
{
        return S{
                2UZ,  3UZ,  5UZ,  7UZ, 11UZ, 13UZ, 17UZ, 19UZ, 23UZ, 29UZ,
               31UZ, 37UZ, 41UZ, 43UZ, 47UZ, 53UZ, 59UZ, 61UZ, 67UZ, 71UZ,
               73UZ, 79UZ, 83UZ, 89UZ, 97UZ
        };
}

// The same pipeline filter_twins builds, but stopping short of ranges::to,
// counted by hand. Separates "the view is broken" from "materialising it is".
static auto count_twins_without_to(S const& primes)
{
        auto view = primes
                | std::views::adjacent<3>
                | std::views::filter([](auto&& x) { auto&& [ prev, self, next ] = x; return self - 2 == prev or self + 2 == next; })
                | std::views::elements<1>
        ;
        auto n = 0UZ;
        for ([[maybe_unused]] auto v : view) {
                ++n;
        }
        return n;
}

// 1. Does the sieve itself survive? If this crashes, filter_twins is innocent
//    and sift_primes1 hands back a broken set.
BOOST_AUTO_TEST_CASE(SiftOnly)
{
        auto const primes = xstd::sift_primes1<S>(N);
        BOOST_CHECK_EQUAL(primes.size(), 25UZ);
}

// 2. The view alone, over a literal set. No sieve, no ranges::to.
BOOST_AUTO_TEST_CASE(ViewOverLiteral)
{
        BOOST_CHECK_EQUAL(count_twins_without_to(known_primes()), 15UZ);
}

// 3. The view alone, over a sifted set. Differs from 2 only in where the set
//    came from, so a crash here and not in 2 indicts sift_primes1's output.
BOOST_AUTO_TEST_CASE(ViewOverSifted)
{
        auto const primes = xstd::sift_primes1<S>(N);
        BOOST_CHECK_EQUAL(count_twins_without_to(primes), 15UZ);
}

// 4. The whole of filter_twins, ranges::to included, over a literal set.
//    A crash here and not in 2 indicts ranges::to rather than the view.
BOOST_AUTO_TEST_CASE(FilterTwinsOverLiteral)
{
        auto const twins = xstd::filter_twins(known_primes());
        BOOST_CHECK_EQUAL(twins.size(), 15UZ);
}

// 5. Exactly what test.set.sieve does at the line that crashes.
BOOST_AUTO_TEST_CASE(FilterTwinsOverSifted)
{
        auto const primes = xstd::sift_primes1<S>(N);
        auto const twins = xstd::filter_twins(primes);
        BOOST_CHECK_EQUAL(twins.size(), 15UZ);
}

BOOST_AUTO_TEST_SUITE_END()
