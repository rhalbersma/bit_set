#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset/factory.hpp>   // make_bitset
#include <concepts.hpp>         // resizeable
#include <algorithm>            // max
#include <ranges>               // cartesian_product, iota

#if defined(_MSC_VER)
        // std::bitset<0> and xstd::bit_set<0> give bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

template<class T, auto Limit>
inline constexpr auto limit_v = resizeable<T> ? Limit : T().size();

inline constexpr auto L0 = 128uz;
inline constexpr auto L1 =  64uz;
inline constexpr auto L2 =  32uz;
inline constexpr auto L3 =  16uz;
inline constexpr auto L4 =   8uz;

// NOTE: these tests are O(1)

template<class T, auto N = limit_v<T, L0>>
auto empty_set(auto fun)
{
        auto a = make_bitset<T>(N); [[assume(a.none())]];
        fun(a);
}

template<class T, auto N = limit_v<T, L0>>
auto full_set(auto fun)
{
        auto a = make_bitset<T>(N, true); [[assume(a.all())]];
        fun(a);
}

template<class T, auto N = limit_v<T, L0>>
auto empty_set_pair(auto fun)
{
        auto a = make_bitset<T>(N); [[assume(a.none())]];
        auto b = make_bitset<T>(N); [[assume(b.none())]];
        fun(a, b);
}

// NOTE: these tests are O(N)

template<class T, auto N = limit_v<T, L1>>
auto all_valid(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                fun(i);
        }
}

template<class T, auto N = limit_v<T, L1>>
auto any_value(auto fun)
{
        for (auto i : std::views::iota(0uz, N + 1)) {
                fun(i);
        }
}

template<class T, auto N = limit_v<T, L1>>
auto all_cardinality_sets(auto fun)
{
        for (auto i : std::views::iota(0uz, N + 1)) {
                auto a = make_bitset<T>(N);
                for (auto j : std::views::iota(0uz, i)) {
                        a.set(j);
                }
                [[assume(a.count() == i)]];
                fun(a);
        }
}

template<class T, auto N = limit_v<T, L1>>
auto all_singleton_sets(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                auto a = make_bitset<T>(N); a.set(i); [[assume(a.count() == 1)]];
                fun(a);
        }
}

// NOTE: these tests are O(N^2)

template<class T, auto N = limit_v<T, L2>>
auto all_singleton_set_pairs(auto fun)
{
        for (auto [ i, j ] : std::views::cartesian_product(
                std::views::iota(0uz, N),
                std::views::iota(0uz, N))
        ) {
                auto a = make_bitset<T>(N); a.set(i); [[assume(a.count() == 1)]];
                auto b = make_bitset<T>(N); b.set(j); [[assume(b.count() == 1)]];
                fun(a, b);
        }
}

// NOTE: this test is O(N^3)

template<class T, auto N = limit_v<T, L3>>
auto all_singleton_set_triples(auto fun)
{
        for (auto [ i, j, k ] : std::views::cartesian_product(
                std::views::iota(0uz, N),
                std::views::iota(0uz, N),
                std::views::iota(0uz, N)
        )) {
                auto a = make_bitset<T>(N); a.set(i); [[assume(a.count() == 1)]];
                auto b = make_bitset<T>(N); b.set(j); [[assume(b.count() == 1)]];
                auto c = make_bitset<T>(N); c.set(k); [[assume(c.count() == 1)]];
                fun(a, b, c);
        }
}

// NOTE: this test is O(N^4)

template<class T, auto N = limit_v<T, L4>>
auto all_doubleton_set_pairs(auto fun)
{
        for (auto [ j, n ] : std::views::cartesian_product(
                std::views::iota(1uz, std::ranges::max(N, 1uz)),
                std::views::iota(1uz, std::ranges::max(N, 1uz))
        )) {
                for (auto [ i, m ] : std::views::cartesian_product(
                        std::views::iota(0uz, j),
                        std::views::iota(0uz, n)
                )) {
                        auto a = make_bitset<T>(N); a.set(i); a.set(j); [[assume(a.count() == 2)]];
                        auto b = make_bitset<T>(N); b.set(m); b.set(n); [[assume(b.count() == 2)]];
                        fun(a, b);
                }
        }
}

}       // namespace xstd
