#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <concepts>     // integral
#include <cstddef>      // size_t
#include <iterator>     // next
#include <ranges>       // to
                        // iota, stride, take_while (stride only when __cpp_lib_ranges_stride)

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

// See ext/bitset's opt/bitset/sieve.hpp for why this is guarded on
// __cpp_lib_ranges_stride rather than given a portable fallback: it exists
// specifically to benchmark the adaptor-based style against sift_primes1's
// hand-rolled equivalent below.
#if defined(__cpp_lib_ranges_stride)
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
#endif

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

// Not std::views::adjacent<3> (still missing in libc++ as of Xcode 26.5) -
// unlike sift_primes0/sift_primes1 above, there's no existing hand-rolled
// variant of this to fall back to, so this is a genuine portable rewrite
// rather than a guard: a plain three-iterator walk over consecutive
// (prev, self, next) triples, keeping self whenever it's part of a twin
// prime pair - the same windowing views::adjacent<3> | views::filter |
// views::elements<1> expressed, just without the adaptor.
template<class X>
auto filter_twins(X const& primes)
{
        X result;
        auto first = primes.begin();
        auto last  = primes.end();
        if (first != last) {
                auto prev = first;
                if (auto self = std::next(prev); self != last) {
                        for (auto next = std::next(self); next != last; ++next) {
                                if (*self - 2 == *prev or *self + 2 == *next) {
                                        result.insert(*self);
                                }
                                prev = self;
                                self = next;
                        }
                }
        }
        return result;
}

}       // namespace xstd
