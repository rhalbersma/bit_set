#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ranges>   // adjacent, elements, filter, iota, stride, take_while, to

namespace xstd {

template<class C>
auto sift(C& primes, std::size_t m)
{
        primes.erase(m);
}

template<class C>
struct generate_candidates
{
        auto operator()(std::size_t n) const
        {
                return std::views::iota(2uz, n) | std::ranges::to<C>();
        }
};

template<class C>
auto sift_primes(std::size_t n)
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
auto sift_primes2(std::size_t n)
{    
        auto primes = generate_candidates<C>()(n);
        for (auto p : primes) {
                auto const p_squared = p * p;
                if (p_squared > n) { 
                        break; 
                }
                for (auto m = p_squared; m < n; m += p) {
                        sift(primes, m);
                }
        }
        return primes;
}

template<class C>
auto filter_twins(C const& primes)
{
        return primes
                | std::views::adjacent<3>
                | std::views::filter([](auto&& x) { auto&& [ prev, self, next ] = x; return prev + 2 == self || self + 2 == next; })
                | std::views::elements<1>
                | std::ranges::to<C>()
        ;
}

}       // namespace xstd
