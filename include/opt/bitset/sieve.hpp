#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy/bidirectional.hpp> // view
#include <boost/dynamic_bitset_fwd.hpp> // dynamic_bitset
#include <cstddef>                      // size_t
#include <ranges>                       // iota, stride, take_while

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

// views::stride (P2442R1) isn't in libc++ yet (confirmed missing as of
// Xcode 26.5, still WIP upstream) - guarded on its feature-test macro
// rather than given a portable fallback, since sift_primes0 exists
// specifically to benchmark the adaptor-based style against sift_primes1's
// hand-rolled equivalent below; a fallback loop here would just be
// sift_primes1 again; wouldn't benchmark anything the other doesn't
// already cover.
#if defined(__cpp_lib_ranges_stride)
template<class X>
auto sift_primes0(std::size_t n)
{
        auto primes = generate_candidates<X>()(n);
        for (std::size_t p
                : proxy::bidirectional::view(primes)
                | std::views::take_while([&](std::size_t x) { return x * x < n; })
        ) {
                for (auto m
                        : std::views::iota(p * p, n)
                        | std::views::stride(static_cast<std::ptrdiff_t>(p))
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
        for (std::size_t p : proxy::bidirectional::view(primes)) {
                if (std::size_t m = p * p; m < n) {
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
