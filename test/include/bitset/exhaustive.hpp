#pragma once

//          Copyright Rein Halbersma 2014-2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset/factory.hpp>   // make_bitset
#include <concepts.hpp>         // resizeable
#include <cassert>              // assert
#include <cstddef>              // size_t

#if defined(_MSC_VER)
        // std::bitset<0> and xstd::bit_set<0> give bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

template<class T, std::size_t Limit>
inline auto const limit_v = resizeable<T> ? Limit : T().size();

inline constexpr auto L0 = 128;
inline constexpr auto L1 =  64;
inline constexpr auto L2 =  32;
inline constexpr auto L3 =  16;
inline constexpr auto L4 =   8;

// NOTE: these tests are O(1)

template<class T>
auto empty_set(auto fun)
{
        auto const N = limit_v<T, L0>;
        auto bs0 = make_bitset<T>(N); assert(bs0.count() == 0);
        fun(bs0);
}

template<class T>
auto full_set(auto fun)
{
        auto const N = limit_v<T, L0>;
        auto bsN = make_bitset<T>(N, true); assert(bsN.count() == N);
        fun(bsN);
}

// NOTE: these tests are O(N)

template<class T>
auto all_valid(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = decltype(N)(0); i < N; ++i) {
                fun(i);
        }
}

template<class T>
auto any_value(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = decltype(N)(0); i <= N; ++i) {
                fun(i);
        }
}

template<class T>
auto all_cardinality_sets(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = decltype(N)(0); i <= N; ++i) {
                auto bs = make_bitset<T>(N);
                for (auto j = decltype(N)(0); j < i; ++j) {
                        bs.set(j);
                }
                assert(bs.count() == i);
                fun(bs);
        }
}

template<class T>
auto all_singleton_sets(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = decltype(N)(0); i < N; ++i) {
                auto bs1 = make_bitset<T>(N); bs1.set(i); assert(bs1.count() == 1);
                fun(bs1);
        }
}

// NOTE: these tests are O(N^2)

template<class T>
auto all_singleton_set_pairs(auto fun)
{
        auto const N = limit_v<T, L2>;
        for (auto i = decltype(N)(0); i < N; ++i) {
                auto bs1_i = make_bitset<T>(N); bs1_i.set(i); assert(bs1_i.count() == 1);
                for (auto j = decltype(N)(0); j < N; ++j) {
                        auto bs1_j = make_bitset<T>(N); bs1_j.set(j); assert(bs1_j.count() == 1);
                        fun(bs1_i, bs1_j);
                }
        }
}

// NOTE: this test is O(N^3)

template<class T>
auto all_singleton_set_triples(auto fun)
{
        auto const N = limit_v<T, L3>;
        for (auto i = decltype(N)(0); i < N; ++i) {
                auto bs1_i = make_bitset<T>(N); bs1_i.set(i); assert(bs1_i.count() == 1);
                for (auto j = decltype(N)(0); j < N; ++j) {
                        auto bs1_j = make_bitset<T>(N); bs1_j.set(j); assert(bs1_j.count() == 1);
                        for (auto k = decltype(N)(0); k < N; ++k) {
                                auto bs1_k = make_bitset<T>(N); bs1_k.set(k); assert(bs1_k.count() == 1);
                                fun(bs1_i, bs1_j, bs1_k);
                        }
                }
        }
}

// NOTE: this test is O(N^4)

template<class T>
auto all_doubleton_set_pairs(auto fun)
{
        auto const N = limit_v<T, L4>;
        for (auto j = decltype(N)(1); j < N; ++j) {
                for (auto i = decltype(N)(0); i < j; ++i) {
                        auto bs2_ij = make_bitset<T>(N); bs2_ij.set(i); bs2_ij.set(j); assert(bs2_ij.count() == 2);
                        for (auto n = decltype(N)(1); n < N; ++n) {
                                for (auto m = decltype(N)(0); m < n; ++m) {
                                        auto bs2_mn = make_bitset<T>(N); bs2_mn.set(m); bs2_mn.set(n); assert(bs2_mn.count() == 2);
                                        fun(bs2_ij, bs2_mn);
                                }
                        }
                }
        }
}

}       // namespace xstd
