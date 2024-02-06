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
        auto bs0 = make_bitset<T>(N); [[assume(bs0.none())]];
        fun(bs0);
}

template<class T, auto N = limit_v<T, L0>>
auto full_set(auto fun)
{
        auto bsN = make_bitset<T>(N, true); [[assume(bsN.all())]];
        fun(bsN);
}

template<class T, auto N = limit_v<T, L0>>
auto empty_set_pair(auto fun)
{
        auto bs0 = make_bitset<T>(N); [[assume(bs0.none())]];
        auto bs1 = make_bitset<T>(N); [[assume(bs1.none())]];
        fun(bs0, bs1);
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
                auto bs = make_bitset<T>(N);
                for (auto j : std::views::iota(0uz, i)) {
                        bs.set(j);
                }
                [[assume(bs.count() == i)]];
                fun(bs);
        }
}

template<class T, auto N = limit_v<T, L1>>
auto all_singleton_sets(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                auto bs1 = make_bitset<T>(N); bs1.set(i); [[assume(bs1.count() == 1)]];
                fun(bs1);
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
                auto bs1_i = make_bitset<T>(N); bs1_i.set(i); [[assume(bs1_i.count() == 1)]];
                auto bs1_j = make_bitset<T>(N); bs1_j.set(j); [[assume(bs1_j.count() == 1)]];
                fun(bs1_i, bs1_j);
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
                auto bs1_i = make_bitset<T>(N); bs1_i.set(i); [[assume(bs1_i.count() == 1)]];
                auto bs1_j = make_bitset<T>(N); bs1_j.set(j); [[assume(bs1_j.count() == 1)]];
                auto bs1_k = make_bitset<T>(N); bs1_k.set(k); [[assume(bs1_k.count() == 1)]];
                fun(bs1_i, bs1_j, bs1_k);
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
                        auto bs2_ij = make_bitset<T>(N); bs2_ij.set(i); bs2_ij.set(j); [[assume(bs2_ij.count() == 2)]];
                        auto bs2_mn = make_bitset<T>(N); bs2_mn.set(m); bs2_mn.set(n); [[assume(bs2_mn.count() == 2)]];
                        fun(bs2_ij, bs2_mn);
                }
        }
}

}       // namespace xstd
