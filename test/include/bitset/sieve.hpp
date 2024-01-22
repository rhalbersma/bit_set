#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/dynamic_bitset_fwd.hpp> // dynamic_bitset
#include <ranges>                       // iota, stride, take_while
#include <type_traits>                  // conditional_t

namespace xstd {

template<int N, std::unsigned_integral Block> 
        requires (N >= 0) 
class bit_set;

template<class C>
concept legacy_bitset = requires(C& bs, std::size_t pos)
{
        bs.set();
        bs.reset(pos);
};

template<legacy_bitset C>
auto size(C const& c)
{
        return c.count();
}

template<class C>
struct generate_empty
{
        auto operator()(auto) const
        {
                return C();
        }
};

template<std::unsigned_integral Block, class Allocator>
struct generate_empty<boost::dynamic_bitset<Block, Allocator>>
{
        auto operator()(boost::dynamic_bitset<Block, Allocator>::size_type n) const
        {
                return boost::dynamic_bitset<Block, Allocator>(n);
        }
};

template<legacy_bitset C>
auto fill(C& empty)
{
        empty.set();
}

template<int N, std::unsigned_integral Block>
auto fill(xstd::bit_set<N, Block>& empty)
{
        empty.fill();
}

template<legacy_bitset C>
auto sift(C& primes, std::size_t m)
{
        primes.reset(m);
}

template<int N, std::unsigned_integral Block>
auto sift(xstd::bit_set<N, Block>& primes, int m)
{
        primes.pop(m);
}

template<class C>
struct generate_candidates
{       
        auto operator()(auto n) const
        {
                auto candidates = generate_empty<C>()(n);
                fill(candidates);
                sift(candidates, 0);
                sift(candidates, 1);
                return candidates;
        }
};

template<class C>
auto sift_primes(std::conditional_t<legacy_bitset<C>, std::size_t, int> n)
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
        return primes & (primes << 2 | primes >> 2);
}

}       // namespace xstd
