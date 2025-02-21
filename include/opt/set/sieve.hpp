#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <concepts>     // integral
#include <cstddef>      // size_t
#include <ranges>       // to
                        // adjacent, elements, filter, iota, stride, take_while

namespace xstd {

template<class X>
auto sift(X& primes, std::size_t m)
{
        primes.erase(m);
}

template<class X>
struct generate_candidates
{
        auto operator()(std::size_t n) const
        {
                return std::views::iota(2uz, n) | std::ranges::to<X>();
        }
};

template<class X>
auto sift_primes0(std::size_t n)
{
        auto primes = generate_candidates<X>()(n);
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

template<class X>
auto sift_primes1(std::size_t n)
{
        auto primes = generate_candidates<X>()(n);
        for (auto p : primes) {
                if (auto m = p * p; m < n) {
                        do {
                                sift(primes, m);
                                m += p;
                        } while(m < n);
                } else {
                        break;
                }
        }
        return primes;
}

template<class X>
auto filter_twins(X const& primes)
{
        return primes
                | std::views::adjacent<3>
                | std::views::filter([](auto&& x) { auto&& [ prev, self, next ] = x; return self - 2 == prev or self + 2 == next; })
                | std::views::elements<1>
                | std::ranges::to<X>()
        ;
}

}       // namespace xstd
