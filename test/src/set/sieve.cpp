//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <fmt/format.h>                 // format
#include <fmt/ranges.h>
#include <ranges>                       // elements, filter, iota, pairwise, stride, take_while, to
#include <set>                          // set
#include <utility>                      // pair

template<class C>
auto sift(C& primes, int m)
{
        primes.erase(m);
}

template<class C>
struct generate_candidates
{
        auto operator()(int n) const
        {
                return std::views::iota(2, n) | std::ranges::to<C>();
        }
};

template<class C>
auto sift_primes(int n)
{
        auto primes = generate_candidates<C>()(n);
        for (auto p
                : primes
                | std::views::take_while([&](auto x) { return x * x < n; })
        ) {
                for (auto m
                        : std::views::iota(p * p, n)
                        | std::views::stride(p)
                ) {
                        sift(primes, m);
                }
        }
        return primes;
}

template<class C>
auto filter_twins(C const& primes)
{
        return primes
                | std::views::pairwise
                | std::views::filter([](auto&& x) { auto&& [ first, second ] = x; return first + 2 == second; })
                | std::views::elements<0>
                | std::ranges::to<C>()
        ;
}

BOOST_AUTO_TEST_SUITE(Sieve)

inline constexpr auto N = 100;

using set_types = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       xstd::bit_set<N>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Format, C, set_types)
{
        auto const primes = sift_primes<C>(N);
        BOOST_CHECK_EQUAL(fmt::format("{}", primes), "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}");

        auto const twins = filter_twins(primes);
        BOOST_CHECK_EQUAL(fmt::format("{}", twins), "{3, 5, 11, 17, 29, 41, 59, 71}");
}

BOOST_AUTO_TEST_SUITE_END()
