#pragma once

//          Copyright Rein Halbersma 2014-2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <concepts.hpp> // resizeable
#include <cassert>      // assert
#include <cstddef>      // size_t

#if defined(_MSC_VER)
        // std::bitset<0> and xstd::bit_set<0> give bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

template<class BitSet, std::size_t Limit>
inline auto const limit_v = resizeable<BitSet> ? Limit : fn_size(BitSet());

inline constexpr auto L0 = 128;
inline constexpr auto L1 =  64;
inline constexpr auto L2 =  32;
inline constexpr auto L3 =  16;
inline constexpr auto L4 =   8;

// NOTE: these tests are O(1)

template<class BitSet>
auto empty_set(auto fun)
{
        auto const N = limit_v<BitSet, L0>;
        BitSet bs0; resize(bs0, N); assert(count(bs0) == 0);
        fun(bs0);
}

template<class BitSet>
auto full_set(auto fun)
{
        auto const N = limit_v<BitSet, L0>;
        BitSet bsN; resize(bsN, N, true); assert(count(bsN) == N);
        fun(bsN);
}

// NOTE: these tests are O(N)

template<class BitSet>
auto all_valid(auto fun)
{
        auto const N = limit_v<BitSet, L1>;
        for (auto i = decltype(N)(0); i < N; ++i) {
                fun(i);
        }
}

template<class BitSet>
auto any_value(auto fun)
{
        auto const N = limit_v<BitSet, L1>;
        for (auto i = decltype(N)(0); i <= N; ++i) {
                fun(i);
        }
}

template<class BitSet>
auto all_cardinality_sets(auto fun)
{
        auto const N = limit_v<BitSet, L1>;
        for (auto i = decltype(N)(0); i <= N; ++i) {
                BitSet bs; resize(bs, N);
                for (auto j = decltype(N)(0); j < i; ++j) {
                        set(bs, j);
                }
                assert(count(bs) == i);
                fun(bs);
        }
}

template<class BitSet>
auto all_singleton_sets(auto fun)
{
        auto const N = limit_v<BitSet, L1>;
        for (auto i = decltype(N)(0); i < N; ++i) {
                BitSet bs1; resize(bs1, N); set(bs1, i); assert(count(bs1) == 1);
                fun(bs1);
        }
}

// NOTE: these tests are O(N^2)

template<class BitSet>
auto all_singleton_set_pairs(auto fun)
{
        auto const N = limit_v<BitSet, L2>;
        for (auto i = decltype(N)(0); i < N; ++i) {
                BitSet bs1_i; resize(bs1_i, N); set(bs1_i, i); assert(count(bs1_i) == 1);
                for (auto j = decltype(N)(0); j < N; ++j) {
                        BitSet bs1_j; resize(bs1_j, N); set(bs1_j, j); assert(count(bs1_j) == 1);
                        fun(bs1_i, bs1_j);
                }
        }
}

// NOTE: this test is O(N^3)

template<class BitSet>
auto all_singleton_set_triples(auto fun)
{
        auto const N = limit_v<BitSet, L3>;
        for (auto i = decltype(N)(0); i < N; ++i) {
                BitSet bs1_i; resize(bs1_i, N); set(bs1_i, i); assert(count(bs1_i) == 1);
                for (auto j = decltype(N)(0); j < N; ++j) {
                        BitSet bs1_j; resize(bs1_j, N); set(bs1_j, j); assert(count(bs1_j) == 1);
                        for (auto k = decltype(N)(0); k < N; ++k) {
                                BitSet bs1_k; resize(bs1_k, N); set(bs1_k, k); assert(count(bs1_k) == 1);
                                fun(bs1_i, bs1_j, bs1_k);
                        }
                }
        }
}

// NOTE: this test is O(N^4)

template<class BitSet>
auto all_doubleton_set_pairs(auto fun)
{
        auto const N = limit_v<BitSet, L4>;
        for (auto j = decltype(N)(1); j < N; ++j) {
                for (auto i = decltype(N)(0); i < j; ++i) {
                        BitSet bs2_ij; resize(bs2_ij, N); set(bs2_ij, i); set(bs2_ij, j); assert(count(bs2_ij) == 2);
                        for (auto n = decltype(N)(1); n < N; ++n) {
                                for (auto m = decltype(N)(0); m < n; ++m) {
                                        BitSet bs2_mn; resize(bs2_mn, N); set(bs2_mn, m); set(bs2_mn, n); assert(count(bs2_mn) == 2);
                                        fun(bs2_ij, bs2_mn);
                                }
                        }
                }
        }
}

}       // namespace xstd
