//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>             // bit_set
#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/std/ranges.hpp>           // as_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <fmt/format.h>                 // format
#include <fmt/ranges.h>
#include <ranges>                       // iota, stride, take_while

BOOST_AUTO_TEST_SUITE(Sieve)

inline constexpr auto format_primes = "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}";
inline constexpr auto format_twins  = "{3, 5, 11, 17, 29, 41, 59, 71}";

BOOST_AUTO_TEST_CASE(XstdBitSet)
{
        constexpr auto N = 100;
        using set_type = xstd::bit_set<N>;

        set_type primes;
        primes.fill();
        primes.erase(0);
        primes.erase(1);

        for (auto p : primes | std::views::take_while([](auto x) { return x * x < N; })) {
                for (auto m : std::views::iota(p * p, N) | std::views::stride(p)) {
                        primes.erase(m);
                }
        }

        auto const twins = primes & primes >> 2;

        BOOST_CHECK_EQUAL(fmt::format("{}", primes), format_primes);
        BOOST_CHECK_EQUAL(fmt::format("{}", twins),  format_twins);
}

BOOST_AUTO_TEST_CASE(StdBitset)
{
        constexpr auto N = 100uz;
        using set_type = std::bitset<N>;

        set_type primes;
        primes.set();
        primes.reset(0);
        primes.reset(1);

        for (auto p : primes | std::views::take_while([](auto x) { return x * x < N; })) {
                for (auto m : std::views::iota(p * p, N) | std::views::stride(p)) {
                        primes.reset(m);
                }
        }

        auto const twins = primes & primes >> 2;

        BOOST_CHECK_EQUAL(fmt::format("{}", primes | xstd::views::as_set), format_primes);
        BOOST_CHECK_EQUAL(fmt::format("{}", twins  | xstd::views::as_set), format_twins);
}

BOOST_AUTO_TEST_CASE(BoostDynamicBitset)
{
        constexpr auto N = 100uz;
        using set_type = boost::dynamic_bitset<>;

        set_type primes(N);
        primes.set();
        primes.reset(0);
        primes.reset(1);

        for (auto p : primes | std::views::take_while([](auto x) { return x * x < N; })) {
                for (auto m : std::views::iota(p * p, N) | std::views::stride(p)) {
                        primes.reset(m);
                }
        }

        auto const twins = primes & primes >> 2;

        BOOST_CHECK_EQUAL(fmt::format("{}", primes | xstd::views::as_set), format_primes);
        BOOST_CHECK_EQUAL(fmt::format("{}", twins  | xstd::views::as_set), format_twins);
}

BOOST_AUTO_TEST_SUITE_END()
