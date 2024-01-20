//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/std/ranges.hpp>           // as_set
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <fmt/format.h>                 // format
#include <fmt/ranges.h>
#include <ranges>                       // iota, stride, take_while
#include <type_traits>                  // conditional_t
#include <utility>                      // pair

template<class C>
concept bitset = requires(C& bs, std::size_t pos)
{
        bs.set();
        bs.reset(pos);
};

template<class C, auto>
struct generate_empty
{
        auto operator()() const
        {
                return C();
        }
};

template<std::unsigned_integral Block, class Allocator, std::size_t N>
struct generate_empty<boost::dynamic_bitset<Block, Allocator>, N>
{
        auto operator()() const
        {
                return boost::dynamic_bitset<Block, Allocator>(N);
        }
};

template<int N, std::unsigned_integral Block>
auto fill(xstd::bit_set<N, Block>& empty)
{
        empty.fill();
}

template<bitset C>
auto fill(C& empty)
{
        empty.set();
}

template<int N, std::unsigned_integral Block>
auto sift(xstd::bit_set<N, Block>& primes, int m)
{
        primes.erase(m);
}

template<bitset C>
auto sift(C& primes, std::size_t m)
{
        primes.reset(m);
}

template<class C, std::conditional_t<bitset<C>, std::size_t, int> N>
struct generate_candidates
{       
        auto operator()() const
                -> std::pair<C, decltype(N)> 
        {
                auto candidates = generate_empty<C, N>()();
                fill(candidates);
                sift(candidates, 0);
                sift(candidates, 1);
                return { candidates, N };
        }
};

template<class G>
auto sift_primes(G candidates)
{    
        auto [ primes, N ] = candidates();
        for (auto p 
                : primes 
                | std::views::take_while([&](auto x) { return x * x < N; })
        ) {
                for (auto m 
                        : std::views::iota(p * p, N) 
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
        return primes & primes >> 2;
}

BOOST_AUTO_TEST_SUITE(Sieve)

inline constexpr auto N = 100;

using set_types = boost::mpl::vector
<       std::bitset<N>
,       boost::dynamic_bitset<>
,       xstd::bit_set<N>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Format, C, set_types)
{
        auto const primes = sift_primes(generate_candidates<C, N>());
        BOOST_CHECK_EQUAL(fmt::format("{}", primes | xstd::views::as_set), "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}");

        auto const twins = filter_twins(primes);
        BOOST_CHECK_EQUAL(fmt::format("{}", twins  | xstd::views::as_set), "{3, 5, 11, 17, 29, 41, 59, 71}");
}

BOOST_AUTO_TEST_SUITE_END()
