#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset/factory.hpp>   // make_bitset
#include <concepts.hpp>         // dynamic
#include <algorithm>            // max
#include <cassert>              // assert
#include <ranges>               // cartesian_product, iota

#if defined(_MSC_VER)
        // std::bitset<0> and xstd::bit_set<0> give bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

template<class X, auto Limit>
inline constexpr auto limit_v = dynamic<X> ? Limit : X().size();

inline constexpr auto L0 = 128uz;
inline constexpr auto L1 =  64uz;
inline constexpr auto L2 =  32uz;
inline constexpr auto L3 =  16uz;
inline constexpr auto L4 =   8uz;

// NOTE: these tests are O(1)

template<class X, auto N = limit_v<X, L0>>
auto empty_set(auto fun)
{
        auto a = make_bitset<X>(N); assert(a.none());
        fun(a);
}

template<class X, auto N = limit_v<X, L0>>
auto full_set(auto fun)
{
        auto a = make_bitset<X>(N, true); assert(a.all());
        fun(a);
}

template<class X, auto N = limit_v<X, L0>>
auto empty_set_pair(auto fun)
{
        auto a = make_bitset<X>(N); assert(a.none());
        auto b = make_bitset<X>(N); assert(b.none());
        fun(a, b);
}

// NOTE: these tests are O(N)

template<class X, auto N = limit_v<X, L1>>
auto all_valid(auto fun)
{
        for (auto i : std::views::iota(std::size_t(0), N)) {
                fun(i);
        }
}

template<class X, auto N = limit_v<X, L1>>
auto any_value(auto fun)
{
        for (auto i : std::views::iota(std::size_t(0), N + 1)) {
                fun(i);
        }
}

template<class X, auto N = limit_v<X, L1>>
auto all_cardinality_sets(auto fun)
{
        for (auto i : std::views::iota(std::size_t(0), N + 1)) {
                auto a = make_bitset<X>(N);
                for (auto j : std::views::iota(std::size_t(0), i)) {
                        a.set(j);
                }
                assert(a.count() == i);
                fun(a);
        }
}

template<class X, auto N = limit_v<X, L1>>
auto all_singleton_sets(auto fun)
{
        for (auto i : std::views::iota(std::size_t(0), N)) {
                auto a = make_bitset<X>(N); a.set(i); assert(a.count() == 1);
                fun(a);
        }
}

// NOTE: these tests are O(N^2)

template<class X, auto N = limit_v<X, L2>>
auto all_singleton_set_pairs(auto fun)
{
        for (auto [ i, j ] : std::views::cartesian_product(
                std::views::iota(std::size_t(0), N),
                std::views::iota(std::size_t(0), N))
        ) {
                auto a = make_bitset<X>(N); a.set(i); assert(a.count() == 1);
                auto b = make_bitset<X>(N); b.set(j); assert(b.count() == 1);
                fun(a, b);
        }
}

// NOTE: this test is O(N^3)

template<class X, auto N = limit_v<X, L3>>
auto all_singleton_set_triples(auto fun)
{
        for (auto [ i, j, k ] : std::views::cartesian_product(
                std::views::iota(std::size_t(0), N),
                std::views::iota(std::size_t(0), N),
                std::views::iota(std::size_t(0), N)
        )) {
                auto a = make_bitset<X>(N); a.set(i); assert(a.count() == 1);
                auto b = make_bitset<X>(N); b.set(j); assert(b.count() == 1);
                auto c = make_bitset<X>(N); c.set(k); assert(c.count() == 1);
                fun(a, b, c);
        }
}

// NOTE: this test is O(N^4)

template<class X, auto N = limit_v<X, L4>>
auto all_doubleton_set_pairs(auto fun)
{
        for (auto [ j, n ] : std::views::cartesian_product(
                std::views::iota(std::size_t(1), std::ranges::max(N, std::size_t(1))),
                std::views::iota(std::size_t(1), std::ranges::max(N, std::size_t(1)))
        )) {
                for (auto [ i, m ] : std::views::cartesian_product(
                        std::views::iota(std::size_t(0), j),
                        std::views::iota(std::size_t(0), n)
                )) {
                        auto a = make_bitset<X>(N); a.set(i); a.set(j); assert(a.count() == 2);
                        auto b = make_bitset<X>(N); b.set(m); b.set(n); assert(b.count() == 2);
                        fun(a, b);
                }
        }
}

}       // namespace xstd
