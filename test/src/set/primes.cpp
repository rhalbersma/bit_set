//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <fmt/format.h>                 // format
#include <fmt/ranges.h>
#include <xstd/bit_set.hpp>             // bit_set
#include <ranges>                       // elements, filter, iota, pairwise, stride, take_while, to
#include <set>                          // set
#include <tuple>                        // get

BOOST_AUTO_TEST_SUITE(Sieve)

inline constexpr auto N = 100;

using int_set_types = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       xstd::bit_set<N>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(TwinPrimes, set_type, int_set_types)
{
        auto primes = std::views::iota(2, N) | std::ranges::to<set_type>();
        
        for (auto p : primes | std::views::take_while([](auto x) { return x * x < N; })) {
                for (auto m : std::views::iota(p * p, N) | std::views::stride(p)) {
                        primes.erase(m);
                }
        }

        auto const twins = primes | std::views::pairwise | std::views::filter([](auto const& x) { 
                return std::get<0>(x) + 2 == std::get<1>(x);
        }) | std::views::elements<0> | std::ranges::to<set_type>();

        BOOST_CHECK(fmt::format("{}", primes) == "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}");
        BOOST_CHECK(fmt::format("{}", twins)  == "{3, 5, 11, 17, 29, 41, 59, 71}");
}

BOOST_AUTO_TEST_SUITE_END()
