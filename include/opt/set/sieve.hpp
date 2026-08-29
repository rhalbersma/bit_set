#ifndef OPT_SET_SIEVE_HPP
#define OPT_SET_SIEVE_HPP

//          Copyright Rein Halbersma 2014-2025.
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
                return std::views::iota(2UZ, n) | std::ranges::to<X>();
        }
};

// Both sieves iterate a snapshot rather than primes itself, because sift()
// erases from primes and the range-for has already cached that container's
// end(). For a node-based X -- std::set -- erasing invalidates only the erased
// element, and m is always composite while p is not, so the cached end()
// survives. For a vector-backed X -- std::flat_set -- every erase shifts the
// tail and the cached end() is stale from the first sift onwards. MSVC's
// _ITERATOR_DEBUG_LEVEL=2 rejects the comparison outright ("vector iterators
// incompatible", <vector>:203); libstdc++ has no such check, so the MinGW
// legs walk past the real end and segfault, and the Linux legs read
// still-allocated memory and pass.
//
// The contains() guard restores what iterating the shrinking container gave
// for free: p skips values already sifted out. Without it the snapshot also
// offers composite p, whose multiples are all sifted already -- the same
// result for strictly more work.
template<class X>
auto sift_primes0(std::size_t n)
{
        auto primes = generate_candidates<X>()(n);
        auto const candidates = primes;
        for (auto p
                : candidates
                | std::views::take_while([&](auto x) { return x * x < n; })
        ) {
                if (not primes.contains(p)) {
                        continue;
                }
                for (auto m = p * p; m < n; m += p) {
                        sift(primes, m);
                }
        }
        return primes;
}

template<class X>
auto sift_primes1(std::size_t n)
{
        auto primes = generate_candidates<X>()(n);
        auto const candidates = primes;
        for (auto p : candidates) {
                if (auto m = p * p; m < n) {
                        if (not primes.contains(p)) {
                                continue;
                        }
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

#endif  // include guard
