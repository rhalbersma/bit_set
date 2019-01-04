#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>   // has_resize_v
#include <cstddef>      // size_t
#include <utility>      // declval

#if defined(_MSC_VER)
        // std::bitset<0> and xstd::bit_set<0> give bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

template<class T, std::size_t L>
auto const limit_v = tti::has_resize_v<T> ? L : fn_size(T{});

inline constexpr auto L0 = 256;
inline constexpr auto L1 = 128;
inline constexpr auto L2 =  64;
inline constexpr auto L3 =  32;

// NOTE: these tests are O(1)

template<class BitSet, class UnaryFunction>
auto empty_set(UnaryFunction fun)
{
        auto const N = limit_v<BitSet, L0>;
        BitSet bs0; resize(bs0, N);
        fun(bs0);
}

template<class BitSet, class UnaryFunction>
auto full_set(UnaryFunction fun)
{
        auto const N = limit_v<BitSet, L0>;
        BitSet bsN; resize(bsN, N, true);
        fun(bsN);
}

// NOTE: these tests are O(N)

template<class BitSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        auto const N = limit_v<BitSet, L1>;
        for (auto i = decltype(N){0}; i < N; ++i) {
                fun(i);
        }
}

template<class BitSet, class UnaryFunction>
auto all_cardinality_sets(UnaryFunction fun)
{
        auto const N = limit_v<BitSet, L1>;
        for (auto i = decltype(N){0}; i <= N; ++i) {
                BitSet bs; resize(bs, N);
                for (auto j = decltype(N){0}; j < i; ++j) {
                        set(bs, j);
                }
                assert(count(bs) == i);
                fun(bs);
        }
}

template<class BitSet, class UnaryFunction>
auto all_singleton_sets(UnaryFunction fun)
{
        auto const N = limit_v<BitSet, L1>;
        for (auto i = decltype(N){0}; i < N; ++i) {
                BitSet bs1; resize(bs1, N); set(bs1, i);
                fun(bs1);
        }
}

// NOTE: these tests are O(N^2)

template<class BitSet, class BinaryFunction>
auto all_singleton_set_pairs(BinaryFunction fun)
{
        auto const N = limit_v<BitSet, L2>;
        for (auto i = decltype(N){0}; i < N; ++i) {
                BitSet bs1_i; resize(bs1_i, N); set(bs1_i, i);
                for (auto j = decltype(N){0}; j < N; ++j) {
                        BitSet bs1_j; resize(bs1_j, N); set(bs1_j, j);
                        fun(bs1_i, bs1_j);
                }
        }
}

// NOTE: this test is O(N^3)

template<class BitSet, class TernaryFunction>
auto all_singleton_set_triples(TernaryFunction fun)
{
        auto const N = limit_v<BitSet, L3>;
        for (auto i = decltype(N){0}; i < N; ++i) {
                BitSet bs1_i; resize(bs1_i, N); set(bs1_i, i);
                for (auto j = decltype(N){0}; j < N; ++j) {
                        BitSet bs1_j; resize(bs1_j, N); set(bs1_j, j);
                        for (auto k = decltype(N){0}; k < N; ++k) {
                                BitSet bs1_k; resize(bs1_k, N); set(bs1_k, k);
                                fun(bs1_i, bs1_j, bs1_k);
                        }
                }
        }
}

}       // namespace xstd
