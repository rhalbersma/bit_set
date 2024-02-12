#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/dynamic_bitset_fwd.hpp> // dynamic_bitset
#include <cstddef>                      // size_t
#include <ranges>                       // iota, stride, take_while
#include <type_traits>                  // conditional_t

namespace xstd {

template<class X>
concept legacy_bitset = requires(X& a, std::size_t pos)
{
        a.set();
        a.reset(pos);
};

template<class X>
concept modern_bitset = requires(X& a, std::size_t pos)
{
        a.fill();
        a.erase(pos);
};

template<class X>
struct generate_empty
{
        auto operator()(std::size_t) const
        {
                return X();
        }
};

template<std::unsigned_integral Block, class Allocator>
struct generate_empty<boost::dynamic_bitset<Block, Allocator>>
{
        auto operator()(std::size_t n) const
        {
                return boost::dynamic_bitset<Block, Allocator>(n);
        }
};

template<class X>
auto fill(X& empty)
{
        if constexpr (legacy_bitset<X>) {
                empty.set();
        } else if constexpr (modern_bitset<X>) {
                empty.fill();
        } else {
                static_assert(false);
        }
}

template<class X>
auto sift(X& primes, std::size_t m)
{
        if constexpr (legacy_bitset<X>) {
                primes.reset(m);
        } else if constexpr (modern_bitset<X>) {
                primes.erase(m);
        } else {
                static_assert(false);
        }
}

template<class X>
struct generate_candidates
{
        auto operator()(auto n) const
        {
                auto candidates = generate_empty<X>()(n);
                fill(candidates);
                sift(candidates, 0);
                sift(candidates, 1);
                return candidates;
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
        return primes & (primes << 2 | primes >> 2);
}

}       // namespace xstd
